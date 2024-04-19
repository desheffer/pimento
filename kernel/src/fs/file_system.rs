use alloc::vec::Vec;

use crate::fs::{Directory, File, Node, NodeLink};
use crate::sync::Arc;

pub const CURRENT: &str = ".";
pub const PARENT: &str = "..";

/// A file system.
pub trait FileSystem {
    /// Gets the node at the root of the file system.
    fn root(&self) -> Arc<Node>;

    /// Makes a directory.
    fn mkdir(&self, _dir: &Arc<Node>, _name: &str) -> Result<(), ()> {
        Err(())
    }

    /// Removes a directory.
    fn rmdir(&self, _dir: &Arc<Node>, _name: &str) -> Result<(), ()> {
        Err(())
    }

    /// Makes a regular file.
    fn mknod(&self, _dir: &Arc<Node>, _name: &str) -> Result<(), ()> {
        Err(())
    }

    /// Unlinks a regular file.
    fn unlink(&self, _dir: &Arc<Node>, _name: &str) -> Result<(), ()> {
        Err(())
    }

    /// Renames a file or directory.
    fn rename(
        &self,
        _old_dir: &Arc<Node>,
        _old_name: &str,
        _new_dir: &Arc<Node>,
        _new_name: &str,
    ) -> Result<(), ()> {
        Err(())
    }

    /// Opens a directory.
    fn opendir(&self, _node: &Arc<Node>) -> Result<(), ()> {
        Err(())
    }

    /// Reads contents from a directory.
    fn readdir(
        &self,
        _directory: &Arc<Directory>,
        _position: usize,
    ) -> Result<Option<NodeLink>, ()> {
        Err(())
    }

    /// Opens a file.
    fn open(&self, _node: &Arc<Node>) -> Result<(), ()> {
        Err(())
    }

    /// Seeks to a new position in a file.
    fn seek(&self, _file: &Arc<File>, _position: usize) -> Result<(), ()> {
        Err(())
    }

    /// Reads bytes from a file.
    fn read(&self, _file: &Arc<File>, _position: usize, _count: usize) -> Result<Vec<u8>, ()> {
        Err(())
    }

    /// Writes bytes to a file.
    fn write(&self, _file: &Arc<File>, _position: usize, _buf: &[u8]) -> Result<usize, ()> {
        Err(())
    }
}
