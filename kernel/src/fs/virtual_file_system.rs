use alloc::vec::Vec;

use crate::fs::{Directory, File, FileSystem, Node, NodeLink, NodeType, Tmpfs, CURRENT, PARENT};
use crate::sync::{Arc, Mutex};

/// A record that `subtree` is mounted on `mount_point`.
struct Mount {
    mount_point: Arc<Node>,
    subtree: Arc<dyn FileSystem>,
}

impl Mount {
    /// Creates a mount record.
    fn new(mount_point: Arc<Node>, subtree: Arc<dyn FileSystem>) -> Self {
        Self {
            mount_point,
            subtree,
        }
    }
}

/// A virtual file system.
pub struct VirtualFileSystem {
    root_file_system: Arc<Tmpfs>,
    mounts: Mutex<Vec<Mount>>,
}

impl VirtualFileSystem {
    /// Creates a virtual file system.
    pub fn new() -> Self {
        Self {
            root_file_system: Tmpfs::new(),
            mounts: Mutex::new(Vec::new()),
        }
    }

    /// Gets the node at the root of the virtual file system.
    pub fn root(&self) -> Arc<Node> {
        self.root_file_system.root()
    }

    /// Resolves a node to its lowest mount point.
    ///
    /// For example, if B is mounted on A, then A is the lowest mount point for either.
    fn resolve_mount_point(&self, node: &Arc<Node>) -> Result<(Arc<Node>, bool), ()> {
        let mounts = self.mounts.lock();

        let mut node = node.clone();
        let mut changed = false;

        loop {
            let file_system = node.file_system().upgrade().ok_or(())?;
            if node != file_system.root() {
                break;
            }

            if let Some(mount) = mounts
                .iter()
                .find(|mount| Arc::ptr_eq(&mount.subtree, &file_system))
            {
                node = mount.mount_point.clone();
                changed = true;
            } else {
                break;
            }
        }

        Ok((node, changed))
    }

    /// Resolves a node to its highest subtree.
    ///
    /// For example, if B is mounted on A, then B is the highest subtree for either.
    fn resolve_subtree(&self, node: &Arc<Node>) -> Result<(Arc<Node>, bool), ()> {
        let mounts = self.mounts.lock();

        let mut node = node.clone();
        let mut changed = false;

        while let Some(mount) = mounts.iter().find(|mount| mount.mount_point == node) {
            node = mount.subtree.root();
            changed = true;
        }

        Ok((node, changed))
    }

    /// Mounts a file system.
    pub fn mount(&self, mount_point: &Arc<Node>, subtree: Arc<dyn FileSystem>) -> Result<(), ()> {
        let (mount_point, _) = self.resolve_subtree(mount_point)?;

        let mut mounts = self.mounts.lock();

        // Only directories can be mount points.
        if !matches!(mount_point.node_type(), NodeType::Directory) {
            return Err(());
        }

        // A subtree cannot have multiple mount points.
        if mounts
            .iter()
            .any(|mount| Arc::ptr_eq(&mount.subtree, &subtree))
        {
            return Err(());
        }

        // Record the mount record.
        mounts.push(Mount::new(mount_point, subtree));

        Ok(())
    }

    /// Makes a directory.
    pub fn mkdir(&self, parent: &Arc<Node>, name: &str) -> Result<(), ()> {
        let (parent, _) = self.resolve_subtree(parent)?;

        if !matches!(parent.node_type(), NodeType::Directory) {
            return Err(());
        }

        if name == CURRENT || name == PARENT {
            return Err(());
        }

        let file_system = parent.file_system().upgrade().ok_or(())?;
        file_system.mkdir(&parent, name)
    }

    /// Removes a directory.
    pub fn rmdir(&self, parent: &Arc<Node>, name: &str) -> Result<(), ()> {
        let (parent, _) = self.resolve_subtree(parent)?;

        if !matches!(parent.node_type(), NodeType::Directory) {
            return Err(());
        }

        if name == CURRENT || name == PARENT {
            return Err(());
        }

        let file_system = parent.file_system().upgrade().ok_or(())?;
        file_system.rmdir(&parent, name)
    }

    /// Makes a regular file.
    pub fn mknod(&self, parent: &Arc<Node>, name: &str) -> Result<(), ()> {
        let (parent, _) = self.resolve_subtree(parent)?;

        if !matches!(parent.node_type(), NodeType::Directory) {
            return Err(());
        }

        if name == CURRENT || name == PARENT {
            return Err(());
        }

        let file_system = parent.file_system().upgrade().ok_or(())?;
        file_system.mknod(&parent, name)
    }

    /// Unlinks a regular file.
    pub fn unlink(&self, parent: &Arc<Node>, name: &str) -> Result<(), ()> {
        let (parent, _) = self.resolve_subtree(parent)?;

        if !matches!(parent.node_type(), NodeType::Directory) {
            return Err(());
        }

        if name == CURRENT || name == PARENT {
            return Err(());
        }

        let file_system = parent.file_system().upgrade().ok_or(())?;
        file_system.rmdir(&parent, name)
    }

    /// Renames a file or directory.
    pub fn rename(
        &self,
        _old_dir: &Arc<Node>,
        _old_name: &str,
        _new_dir: &Arc<Node>,
        _new_name: &str,
    ) -> Result<(), ()> {
        todo!()
    }

    /// Opens a directory.
    pub fn opendir(&self, node: &Arc<Node>) -> Result<Arc<Directory>, ()> {
        let (node, _) = self.resolve_subtree(node)?;

        if !matches!(node.node_type(), NodeType::Directory) {
            return Err(());
        }

        let file_system = node.file_system().upgrade().ok_or(())?;
        file_system.opendir(&node)?;

        Ok(Directory::new(node.clone()))
    }

    /// Opens a file.
    pub fn open(&self, node: &Arc<Node>) -> Result<Arc<File>, ()> {
        let (node, _) = self.resolve_subtree(node)?;

        if matches!(node.node_type(), NodeType::Directory) {
            return Err(());
        }

        let file_system = node.file_system().upgrade().ok_or(())?;
        file_system.open(&node)?;

        Ok(File::new(node.clone()))
    }

    /// Reads contents from the directory.
    pub fn readdir(&self, directory: &Arc<Directory>) -> Result<Option<NodeLink>, ()> {
        let readdir = directory.readdir();

        // If the link points to a parent directory, and if the parent directory is the root node
        // of a subtree, then link to the parent directory of the mount point instead.
        if let Ok(Some(ref node_link)) = readdir {
            if node_link.name() == PARENT {
                let (mount_point, changed) = self.resolve_mount_point(&directory.node())?;
                if changed {
                    let mount_point_dir = self.opendir(&mount_point)?;

                    while let Ok(Some(node_link)) = mount_point_dir.readdir() {
                        if node_link.name() == PARENT {
                            return Ok(Some(node_link));
                        }
                    }

                    return Err(());
                }
            }
        }

        readdir
    }
}
