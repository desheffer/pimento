use alloc::borrow::ToOwned;
use alloc::collections::BTreeMap;
use alloc::vec;
use alloc::vec::Vec;

use crate::fs::{Directory, File, FileSystem, Node, NodeId, NodeLink, NodeType, CURRENT, PARENT};
use crate::sync::{Arc, AtomicCounter, Mutex, Weak};

/// A temporary file system.
// TODO: The heap has a global limit of 4 MB. For file data, use a data structure that is backed by
// `PageAllocator` and can grow more freely.
pub struct Tmpfs {
    me: Weak<Tmpfs>,
    root: Arc<Node>,
    node_counter: AtomicCounter,
    directories: Mutex<BTreeMap<NodeId, Vec<NodeLink>>>,
    files: Mutex<BTreeMap<NodeId, Vec<u8>>>,
}

impl Tmpfs {
    /// Creates a temporary file system.
    pub fn new() -> Arc<Self> {
        Arc::new_cyclic(|me: &Weak<Tmpfs>| {
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
                files: Mutex::new(BTreeMap::new()),
            }
        })
    }
}

impl FileSystem for Tmpfs {
    fn root(&self) -> Arc<Node> {
        self.root.clone()
    }

    fn mkdir(&self, dir: &Arc<Node>, name: &str) -> Result<(), ()> {
        let mut directories = self.directories.lock();

        let node_id = self.node_counter.inc();
        let child_node = Node::new(dir.file_system(), node_id, NodeType::Directory);

        directories
            .get_mut(&dir.node_id())
            .ok_or(())?
            .push(NodeLink::new(name.to_owned(), child_node.clone()));

        directories.insert(
            node_id,
            vec![
                NodeLink::new(CURRENT.to_owned(), child_node.clone()),
                NodeLink::new(PARENT.to_owned(), dir.clone()),
            ],
        );

        directories
            .get_mut(&dir.node_id())
            .ok_or(())?
            .push(NodeLink::new(name.to_owned(), child_node));

        Ok(())
    }

    fn rmdir(&self, _dir: &Arc<Node>, _name: &str) -> Result<(), ()> {
        todo!()
    }

    fn mknod(&self, dir: &Arc<Node>, name: &str) -> Result<(), ()> {
        let mut directories = self.directories.lock();
        let mut files = self.files.lock();

        let node_id = self.node_counter.inc();
        let child_node = Node::new(dir.file_system(), node_id, NodeType::File);

        files.insert(node_id, Vec::new());

        directories
            .get_mut(&dir.node_id())
            .ok_or(())?
            .push(NodeLink::new(name.to_owned(), child_node));

        Ok(())
    }

    fn unlink(&self, _dir: &Arc<Node>, _name: &str) -> Result<(), ()> {
        todo!()
    }

    fn rename(
        &self,
        _old_dir: &Arc<Node>,
        _old_name: &str,
        _new_dir: &Arc<Node>,
        _new_name: &str,
    ) -> Result<(), ()> {
        todo!()
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

    fn seek(&self, _file: &Arc<File>, _position: usize) -> Result<(), ()> {
        Ok(())
    }

    fn read(&self, file: &Arc<File>, position: usize, count: usize) -> Result<Vec<u8>, ()> {
        let files = self.files.lock();

        let node_id = file.node().node_id();
        let file = files.get(&node_id).ok_or(())?;

        let mut range = position..position + count;
        if range.end > file.len() {
            range.end = file.len();
        }

        Ok(file[range].to_vec())
    }

    fn write(&self, file: &Arc<File>, position: usize, buf: &[u8]) -> Result<usize, ()> {
        let mut files = self.files.lock();

        let node_id = file.node().node_id();
        let file = files.get_mut(&node_id).ok_or(())?;

        let range = position..position + buf.len();
        if range.end > file.len() {
            file.resize_with(range.end, || 0);
        }

        let len = range.len();

        file[range].copy_from_slice(buf);

        Ok(len)
    }
}
