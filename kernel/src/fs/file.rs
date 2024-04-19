use alloc::vec::Vec;

use crate::fs::Node;
use crate::sync::{Arc, Mutex, Weak};

/// An open file.
pub struct File {
    me: Weak<Self>,
    node: Arc<Node>,
    position: Mutex<usize>,
}

impl File {
    // Creates a file.
    pub fn new(node: Arc<Node>) -> Arc<Self> {
        Arc::new_cyclic(|me| Self {
            me: me.clone(),
            node,
            position: Mutex::new(0),
        })
    }

    /// Gets the node that this file belongs to.
    pub fn node(&self) -> Arc<Node> {
        self.node.clone()
    }

    /// Gets the current read/write position in the file.
    pub fn position(&self) -> usize {
        *self.position.lock()
    }

    /// Seeks to a new position in the file.
    pub fn seek(&self, position: usize) -> Result<(), ()> {
        let file_system = self.node().file_system().upgrade().ok_or(())?;
        let me = &self.me.upgrade().ok_or(())?;
        file_system.seek(me, position)?;

        *self.position.lock() = position;

        Ok(())
    }

    /// Reads bytes from the file.
    pub fn read(&self, count: usize) -> Result<Vec<u8>, ()> {
        let mut position = self.position.lock();

        let file_system = self.node().file_system().upgrade().ok_or(())?;
        let me = &self.me.upgrade().ok_or(())?;
        let buf = file_system.read(me, *position, count)?;

        *position += buf.len();

        Ok(buf)
    }

    /// Writes bytes to the file.
    pub fn write(&self, buf: &[u8]) -> Result<usize, ()> {
        let mut position = self.position.lock();

        let file_system = self.node().file_system().upgrade().ok_or(())?;
        let me = &self.me.upgrade().ok_or(())?;
        let len = file_system.write(me, *position, buf)?;

        *position += len;

        Ok(len)
    }
}
