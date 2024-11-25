use alloc::sync::{Arc, Weak};

use crate::fs::FileSystem;

pub type NodeId = u64;

/// Node types.
#[derive(Clone, Copy)]
pub enum NodeType {
    CharacterDevice,
    Directory,
    File,
}

/// The data associated with a node in a file system.
pub struct Node {
    me: Weak<Self>,
    file_system: Weak<dyn FileSystem>,
    node_id: NodeId,
    node_type: NodeType,
}

impl Node {
    // Creates a node.
    pub fn new(
        file_system: Weak<dyn FileSystem>,
        node_id: NodeId,
        node_type: NodeType,
    ) -> Arc<Self> {
        Arc::new_cyclic(|me| Self {
            me: me.clone(),
            file_system,
            node_id,
            node_type,
        })
    }

    /// Gets the file system that this node belongs to.
    pub fn file_system(&self) -> Weak<dyn FileSystem> {
        self.file_system.clone()
    }

    /// Gets the node ID.
    pub fn node_id(&self) -> NodeId {
        self.node_id
    }

    /// Gets the node type.
    pub fn node_type(&self) -> NodeType {
        self.node_type
    }
}

impl PartialEq for Node {
    fn eq(&self, other: &Self) -> bool {
        Weak::ptr_eq(&self.file_system, &other.file_system) && self.node_id == other.node_id
    }
}
