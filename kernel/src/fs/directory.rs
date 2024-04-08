use alloc::string::String;

use crate::fs::Node;
use crate::sync::{Arc, Mutex, Weak};

/// A node linked from a directory.
pub struct NodeLink {
    name: String,
    node: Arc<Node>,
}

impl NodeLink {
    /// Creates a child node.
    pub fn new(name: String, node: Arc<Node>) -> Self {
        Self { name, node }
    }

    /// Gets the name of this node.
    pub fn name(&self) -> &str {
        self.name.as_str()
    }

    /// Gets the node that this directory belongs to.
    pub fn node(&self) -> Arc<Node> {
        self.node.clone()
    }
}

/// An open directory.
pub struct Directory {
    me: Weak<Self>,
    node: Arc<Node>,
    position: Mutex<usize>,
}

impl Directory {
    // Creates a directory.
    pub fn new(node: Arc<Node>) -> Arc<Self> {
        Arc::new_cyclic(|me| Self {
            me: me.clone(),
            node,
            position: Mutex::new(0),
        })
    }

    /// Gets the node that this directory belongs to.
    pub fn node(&self) -> Arc<Node> {
        self.node.clone()
    }

    /// Gets the current read/write position in the directory.
    pub fn position(&self) -> usize {
        *self.position.lock()
    }

    /// Reads contents from the directory.
    pub fn readdir(&self) -> Result<Option<NodeLink>, ()> {
        let mut position = self.position.lock();

        let file_system = self.node().file_system().upgrade().ok_or(())?;
        let me = &self.me.upgrade().ok_or(())?;
        let node_link = file_system.readdir(me, *position)?;

        *position += 1;

        Ok(node_link)
    }
}
