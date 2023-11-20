/// A generic interrupt controller.
pub trait InterruptController {
    fn enable(&self, number: u64);

    fn is_pending(&self, number: u64) -> bool;

    fn clear(&self, number: u64);
}
