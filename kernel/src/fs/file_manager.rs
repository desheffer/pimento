use alloc::borrow::ToOwned;
use alloc::string::String;
use alloc::vec::Vec;

use crate::fs::{Directory, File, FileSystem, Node, VirtualFileSystem, CURRENT, PARENT};
use crate::sync::Arc;

pub const SLASH: char = '/';

/// A path representation.
///
/// This is either a relative path with a starting directory or an absolute path.
pub struct PathInfo {
    tokens: Vec<PathToken>,
    start: PathStart,
}

/// The starting directory of a path representation.
#[derive(Clone)]
pub enum PathStart {
    Root,
    Node(Arc<Node>),
}

/// The tokens that make up a path representation.
#[derive(Clone)]
pub enum PathToken {
    Current,
    Parent,
    Literal(String),
}

impl PathToken {
    /// Converts from a path token to a string.
    pub fn to_str(&self) -> &str {
        match self {
            PathToken::Current => CURRENT,
            PathToken::Parent => PARENT,
            PathToken::Literal(s) => s,
        }
    }

    /// Converts from a string to a path token.
    pub fn from_str(s: &str) -> PathToken {
        match s {
            CURRENT => PathToken::Current,
            PARENT => PathToken::Parent,
            s => PathToken::Literal(s.to_owned()),
        }
    }
}

impl PathInfo {
    /// Creates a path representation.
    pub fn new(path: &str, start: Arc<Node>) -> Self {
        if path.starts_with(SLASH) {
            Self::absolute(path).unwrap()
        } else {
            PathInfo {
                tokens: Self::path_str_to_tokens(path),
                start: PathStart::Node(start),
            }
        }
    }

    /// Creates a path representation based on an absolute path.
    pub fn absolute(path: &str) -> Option<Self> {
        if path.starts_with(SLASH) {
            Some(PathInfo {
                tokens: Self::path_str_to_tokens(path),
                start: PathStart::Root,
            })
        } else {
            None
        }
    }

    /// Converts a path from a string to a vector of tokens.
    fn path_str_to_tokens(path: &str) -> Vec<PathToken> {
        path.split(SLASH)
            .filter_map(|s| match s {
                "" => None,
                s => Some(PathToken::from_str(s)),
            })
            .collect()
    }

    /// Retrieves the starting directory.
    fn start(&self) -> &PathStart {
        &self.start
    }

    /// Retrieves the path as a vector of tokens.
    fn tokens(&self) -> &Vec<PathToken> {
        &self.tokens
    }

    /// Retrieves a path representation for the parent of this path.
    fn parent(&self) -> PathInfo {
        let tokens = match self.tokens.split_last() {
            // If the last token is a literal, then remove it.
            Some((PathToken::Literal(_), first_tokens)) => first_tokens.to_vec(),
            // Otherwise, append a parent token.
            _ => {
                let mut tokens = self.tokens.clone();
                tokens.push(PathToken::Parent);
                tokens
            }
        };

        PathInfo {
            tokens,
            start: self.start.clone(),
        }
    }

    /// Retrieves the last token in this path.
    fn last_token(&self) -> Option<&PathToken> {
        self.tokens.last()
    }
}

/// A file system manager.
pub struct FileManager {
    vfs: &'static VirtualFileSystem,
}

impl FileManager {
    /// Creates a file system manager.
    pub fn new(vfs: &'static VirtualFileSystem) -> Self {
        Self { vfs }
    }

    /// Walks the file system using the given path representation.
    fn walk(&self, path: &PathInfo) -> Result<Arc<Node>, ()> {
        let mut current_node = match path.start() {
            PathStart::Root => self.vfs.root(),
            PathStart::Node(node) => node.clone(),
        };

        'outer: for token in path.tokens() {
            let target_name = token.to_str();

            let directory = self.vfs.opendir(&current_node)?;

            while let Some(node_link) = directory.readdir()? {
                if node_link.name() == target_name {
                    current_node = node_link.node();
                    continue 'outer;
                }
            }

            return Err(());
        }

        Ok(current_node)
    }

    /// Mounts a file system.
    pub fn mount(&self, mount_point: &PathInfo, subtree: Arc<dyn FileSystem>) -> Result<(), ()> {
        let mount_point = self.walk(mount_point)?;

        self.vfs.mount(&mount_point, subtree)
    }

    /// Makes a directory.
    pub fn mkdir(&self, path: &PathInfo) -> Result<(), ()> {
        let parent = self.walk(&path.parent())?;

        if let Some(PathToken::Literal(name)) = path.last_token() {
            self.vfs.mkdir(&parent, name)
        } else {
            Err(())
        }
    }

    /// Removes a directory.
    pub fn rmdir(&self, path: &PathInfo) -> Result<(), ()> {
        let parent = self.walk(&path.parent())?;

        if let Some(PathToken::Literal(name)) = path.last_token() {
            self.vfs.rmdir(&parent, name)
        } else {
            Err(())
        }
    }

    /// Makes a regular file.
    pub fn mknod(&self, path: &PathInfo) -> Result<(), ()> {
        let parent = self.walk(&path.parent())?;

        if let Some(PathToken::Literal(name)) = path.last_token() {
            self.vfs.mknod(&parent, name)
        } else {
            Err(())
        }
    }

    /// Unlinks a regular file.
    pub fn unlink(&self, path: &PathInfo) -> Result<(), ()> {
        let parent = self.walk(&path.parent())?;

        if let Some(PathToken::Literal(name)) = path.last_token() {
            self.vfs.unlink(&parent, name)
        } else {
            Err(())
        }
    }

    /// Renames a file or directory.
    pub fn rename(&self, _old_path: &PathInfo, _new_path: &PathInfo) -> Result<(), ()> {
        todo!()
    }

    /// Opens a directory.
    pub fn opendir(&self, path: &PathInfo) -> Result<Arc<Directory>, ()> {
        let node = self.walk(path)?;

        self.vfs.opendir(&node)
    }

    /// Opens a file.
    pub fn open(&self, path: &PathInfo) -> Result<Arc<File>, ()> {
        let node = self.walk(path)?;

        self.vfs.open(&node)
    }
}
