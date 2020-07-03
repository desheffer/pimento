#include <board/bcm2837_serial.h>
#include <pimento.h>
#include <vfs.h>

/**
 * Output a single character on the serial device.
 */
static void _putc(const char c)
{
    // Wait until transmit FIFO is empty.
    while (!(*BCM2837_AUX_MU_LSR & BCM2837_AUX_MU_LSR_TX_EMPTY));

    *BCM2837_AUX_MU_IO = c;
}

/**
 * Input a single character from the serial device.
 */
static char _getc(void)
{
    // Wait until receive FIFO is ready.
    while (!(*BCM2837_AUX_MU_LSR & BCM2837_AUX_MU_LSR_RX_READY));

    return *BCM2837_AUX_MU_IO;
}

/**
 * Open the serial device.
 */
static int _bcm2837_serial_file_open(struct inode * inode, struct file * file)
{
    (void) inode;
    (void) file;

    return 0;
}

/**
 * Read from the serial device.
 */
static ssize_t _bcm2837_serial_file_read(struct file * file, char * buf,
                                         size_t num, loff_t * off)
{
    (void) file;

    size_t pos = 0;

    while (pos < num) {
        char c = _getc();

        if (c == '\r') {
            // Line feed
            *(buf++) = '\n';
            ++pos;
            _putc('\n');

            break;
        } else if (c == 0x7F) {
            // Backspace
            if (pos > 0) {
                --buf;
                --pos;

                _putc('\b');
                _putc(' ');
                _putc('\b');
            }
        } else {
            *(buf++) = c;
            ++pos;
            _putc(c);
        }

        if (c < 0x20 || c > 0x7F) {
            break;
        }
    }

    *off += pos;

    return pos;
}

/**
 * Write to the serial device.
 */
static ssize_t _bcm2837_serial_file_write(struct file * file, const char * buf,
                                          size_t num, loff_t * off)
{
    (void) file;

    size_t pos = 0;

    while (pos < num) {
        _putc(*buf);

        if (*buf == '\n') {
            _putc('\r');
        }

        ++buf;
        ++pos;
    }

    *off += pos;

    return pos;
}

static struct file_operations _bcm2837_serial_file_operations = {
    .open = _bcm2837_serial_file_open,
    .read = _bcm2837_serial_file_read,
    .write = _bcm2837_serial_file_write,
};

/**
 * Configure the serial device.
 */
static void _bcm2837_serial_configure(void)
{
    // Enable the mini UART.
    *BCM2837_AUX_ENABLES |= BCM2837_AUX_ENABLES_MU;

    // Disable interrupts.
    *BCM2837_AUX_MU_IER = 0;

    // Disable transmitter and receiver.
    *BCM2837_AUX_MU_CNTL = 0;

    // Use 8-bit mode.
    *BCM2837_AUX_MU_LCR = BCM2837_AUX_MU_LCR_8BIT;

    // Set the line to high.
    *BCM2837_AUX_MU_MCR = 0;

    // Set the baud rate.
    // desired_baud = clock_freq / (8 * (aux_mu_baud + 1))
    // desired_baud = 115,200
    // clock_freq = 250,000,000
    *BCM2837_AUX_MU_BAUD = 270;

    // Set GPIO 14/15 to take alternative function 5.
    *BCM2837_GPFSEL1 = (*BCM2837_GPFSEL1 & BCM2837_GPFSEL1_FSEL14_MASK) | BCM2837_GPFSEL1_FSEL14_F5;
    *BCM2837_GPFSEL1 = (*BCM2837_GPFSEL1 & BCM2837_GPFSEL1_FSEL15_MASK) | BCM2837_GPFSEL1_FSEL15_F5;

    // Disable pull up/down.
    *BCM2837_GPPUD = 0;

    // Wait 150 cycles for the control signal.
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    // Enable GPIO 14/15.
    *BCM2837_GPPUDCLK0 = BCM2837_GPPUDCLK0_PUD14 | BCM2837_GPPUDCLK0_PUD15;

    // Wait 150 cycles for the control signal.
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    // Flush GPIO settings.
    *BCM2837_GPPUDCLK0 = 0;

    // Enable the receiver and transmitter.
    *BCM2837_AUX_MU_CNTL = BCM2837_AUX_MU_CNTL_RX | BCM2837_AUX_MU_CNTL_TX;
    *BCM2837_AUX_MU_IIR = BCM2837_AUX_MU_IIR_RX | BCM2837_AUX_MU_IIR_TX;
}

/**
 * Output a single character on the serial device.
 */
static void _kputc(void * data, const char c)
{
    (void) data;

    _putc(c);

    if (c == '\n') {
        _putc('\r');
    }
}

/**
 * Initialize serial I/O on the BCM2837.
 */
void bcm2837_serial_init(void)
{
    _bcm2837_serial_configure();

    struct path * path = vfs_path_create();

    vfs_resolve_path(path, 0, "/dev/ttyS0");

    int res = vfs_mknod(path, 0644);
    if (res < 0) {
        vfs_path_destroy(path);
        return;
    }

    path->child->inode->file_operations = &_bcm2837_serial_file_operations;

    vfs_path_destroy(path);

    set_kputc(_kputc);
}
