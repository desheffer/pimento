/// A generic interrupt controller.
pub trait InterruptController {
    /// Enables the given interrupt.
    fn enable(&self, number: u64);

    /// Returns `true` if the given interrupt is pending.
    fn is_pending(&self, number: u64) -> bool;

    /// Clears the given interrupt.
    fn clear(&self, number: u64);
}
