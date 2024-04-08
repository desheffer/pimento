use alloc::borrow::ToOwned;
use alloc::collections::BTreeMap;
use alloc::vec;
use alloc::vec::Vec;

use crate::device::CharacterDevice;
use crate::fs::{Directory, File, FileSystem, Node, NodeId, NodeLink, NodeType, CURRENT, PARENT};
use crate::sync::{Arc, AtomicCounter, Mutex, Weak};

/// A device file system.
pub struct Devfs {
    me: Weak<Devfs>,
    root: Arc<Node>,
    node_counter: AtomicCounter,
    directories: Mutex<BTreeMap<NodeId, Vec<NodeLink>>>,
    character_devices: Mutex<BTreeMap<NodeId, Arc<dyn CharacterDevice>>>,
}

impl Devfs {
    /// Creates a device file system.
    pub fn new() -> Arc<Self> {
        Arc::new_cyclic(|me: &Weak<Devfs>| {
            let node_id = 0;
            let root = Node::new(me.clone(), node_id, NodeType::Directory);

            let mut directories = BTreeMap::new();
            directories.insert(
                node_id,
                vec![
                    NodeLink::new(CURRENT.to_owned(), root.clone()),
                    NodeLink::new(PARENT.to_owned(), root.clone()),
                ],
            );

            Self {
                me: me.clone(),
                root,
                node_counter: AtomicCounter::new(1),
                directories: Mutex::new(directories),
                character_devices: Mutex::new(BTreeMap::new()),
            }
        })
    }

    /// Registers a character device.
    pub fn register_character_device(
        &self,
        name: &str,
        device: Arc<dyn CharacterDevice>,
    ) -> Result<(), ()> {
        let mut directories = self.directories.lock();
        let mut character_devices = self.character_devices.lock();

        let node_id = self.node_counter.inc();
        let child_node = Node::new(self.root.file_system(), node_id, NodeType::CharacterDevice);

        character_devices.insert(node_id, device);

        directories
            .get_mut(&self.root.node_id())
            .ok_or(())?
            .push(NodeLink::new(name.to_owned(), child_node));

        Ok(())
    }
}

impl FileSystem for Devfs {
    fn root(&self) -> Arc<Node> {
        self.root.clone()
    }

    fn opendir(&self, _node: &Arc<Node>) -> Result<(), ()> {
        Ok(())
    }

    fn readdir(&self, directory: &Arc<Directory>, position: usize) -> Result<Option<NodeLink>, ()> {
        let directories = self.directories.lock();

        let node_id = directory.node().node_id();
        let child_node = directories.get(&node_id).ok_or(())?.get(position);

        if let Some(child_node) = child_node {
            Ok(Some(NodeLink::new(
                child_node.name().to_owned(),
                child_node.node(),
            )))
        } else {
            Ok(None)
        }
    }

    fn open(&self, _node: &Arc<Node>) -> Result<(), ()> {
        Ok(())
    }

    fn read(&self, file: &Arc<File>, _position: usize, count: usize) -> Result<Vec<u8>, ()> {
        let node_id = file.node().node_id();

        match file.node().node_type() {
            NodeType::CharacterDevice => {
                let character_devices = self.character_devices.lock();
                let device = character_devices.get(&node_id).ok_or(())?;
                device.read(count)
            }
            _ => Err(()),
        }
    }

    fn write(&self, file: &Arc<File>, _position: usize, buf: &[u8]) -> Result<usize, ()> {
        let node_id = file.node().node_id();

        match file.node().node_type() {
            NodeType::CharacterDevice => {
                let character_devices = self.character_devices.lock();
                let device = character_devices.get(&node_id).ok_or(())?;
                device.write(buf)
            }
            _ => Err(()),
        }
    }
}
