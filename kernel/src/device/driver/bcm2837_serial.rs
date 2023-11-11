use core::arch::asm;

use crate::device::Logger;
use crate::sync::Lock;

const GPFSEL1: *mut u32 = 0x3F200004 as *mut u32; // GPIO function select 1
const GPPUD: *mut u32 = 0x3F200094 as *mut u32; // GPIO pin pull-up/down enable
const GPPUDCLK0: *mut u32 = 0x3F200098 as *mut u32; // GPIO pin pull-up/down enable clock 0
const AUX_ENABLES: *mut u8 = 0x3F215004 as *mut u8; // Auxiliary enables
const AUX_MU_IO: *mut u8 = 0x3F215040 as *mut u8; // Mini UART I/O data
const AUX_MU_IER: *mut u8 = 0x3F215044 as *mut u8; // Mini UART interrupt enable
const AUX_MU_IIR: *mut u8 = 0x3F215048 as *mut u8; // Mini UART interrupt identify
const AUX_MU_LCR: *mut u8 = 0x3F21504C as *mut u8; // Mini UART line control
const AUX_MU_MCR: *mut u8 = 0x3F215050 as *mut u8; // Mini UART modem control
const AUX_MU_LSR: *mut u8 = 0x3F215054 as *mut u8; // Mini UART line status
const AUX_MU_CNTL: *mut u8 = 0x3F215060 as *mut u8; // Mini UART extra control
const AUX_MU_BAUD: *mut u16 = 0x3F215068 as *mut u16; // Mini UART baud rate

const AUX_ENABLES_MU: u8 = 0b1 << 0; // Enable mini UART

const AUX_MU_LCR_8BIT: u8 = 0b11 << 0; // Enable 8-bit mode

const GPFSEL1_FSEL14_MASK: u32 = 0b111 << 12; // Mask GPIO pin 14 function
const GPFSEL1_FSEL14_F5: u32 = 0b010 << 12; // Set GPIO pin 14 to take function 5

const GPFSEL1_FSEL15_MASK: u32 = 0b111 << 15; // Mask GPIO pin 15 function
const GPFSEL1_FSEL15_F5: u32 = 0b010 << 15; // Set GPIO pin 15 to take function 5

const GPPUDCLK0_PUD14: u32 = 0b1 << 14; // Enable GPIO pin 14
const GPPUDCLK0_PUD15: u32 = 0b1 << 15; // Enable GPIO pin 15

const AUX_MU_CNTL_RX: u8 = 0b1 << 0; // Enable RX
const AUX_MU_CNTL_TX: u8 = 0b1 << 1; // Enable TX

const AUX_MU_IIR_TX: u8 = 0b1 << 0; // Clear TX
const AUX_MU_IIR_RX: u8 = 0b1 << 1; // Clear RX

const AUX_MU_LSR_RX_READY: u8 = 0b1 << 0; // Receiver ready
const AUX_MU_LSR_TX_EMPTY: u8 = 0b1 << 5; // Transmitter empty

/// Broadcom chip used in the Raspberry Pi 3 Model B and others
///
/// This implements basic serial input and output capabilities using this chip.
#[derive(Debug)]
pub struct Bcm2837Serial {
    lock: Lock,
}

impl Bcm2837Serial {
    pub const unsafe fn new() -> Self {
        Self { lock: Lock::new() }
    }

    pub fn init(&self) {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            // Enable the mini UART.
            AUX_ENABLES.write_volatile(AUX_ENABLES.read_volatile() | AUX_ENABLES_MU);

            // Disable interrupts.
            AUX_MU_IER.write_volatile(0);

            // Disable transmitter and receiver.
            AUX_MU_CNTL.write_volatile(0);

            // Use 8-bit mode.
            AUX_MU_LCR.write_volatile(AUX_MU_LCR_8BIT);

            // Set the line to high.
            AUX_MU_MCR.write_volatile(0);

            // Set the baud rate.
            // AUX_MU_BAUD = clock_freq / (8 * desired_baud) - 1
            // clock_freq = 250,000,000
            // desired_baud = 115,200
            AUX_MU_BAUD.write_volatile(270);

            // Set GPIO 14/15 to take alternative function 5.
            GPFSEL1.write_volatile(
                GPFSEL1.read_volatile() & !(GPFSEL1_FSEL14_MASK | GPFSEL1_FSEL15_MASK)
                    | (GPFSEL1_FSEL14_F5 | GPFSEL1_FSEL15_F5),
            );

            // Disable pull up/down.
            GPPUD.write_volatile(0);

            // Wait 150 cycles for the control signal.
            for _ in 0..150 {
                asm!("nop");
            }

            // Enable GPIO 14/15.
            GPPUDCLK0.write_volatile(GPPUDCLK0_PUD14 | GPPUDCLK0_PUD15);

            // Wait 150 cycles for the control signal.
            for _ in 0..150 {
                asm!("nop");
            }

            // Flush GPIO settings.
            GPPUDCLK0.write_volatile(0);

            // Enable the receiver and transmitter.
            AUX_MU_CNTL.write_volatile(AUX_MU_CNTL_RX | AUX_MU_CNTL_TX);
            AUX_MU_IIR.write_volatile(AUX_MU_IIR_RX | AUX_MU_IIR_TX);
        });
    }

    fn read_byte(&self) -> Option<u8> {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            // Check if receiver is ready.
            if AUX_MU_LSR.read_volatile() & AUX_MU_LSR_RX_READY == 0 {
                None
            } else {
                Some(AUX_MU_IO.read_volatile())
            }
        })
    }
}

impl Logger for Bcm2837Serial {
    fn write_str(&self, s: &str) {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            for c in s.bytes() {
                // Wait until transmitter is empty.
                while AUX_MU_LSR.read_volatile() & AUX_MU_LSR_TX_EMPTY == 0 {}

                AUX_MU_IO.write_volatile(c as u8);
            }
        });
    }
}
