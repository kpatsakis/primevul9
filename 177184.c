void ext4_xattr_inode_set_class(struct inode *ea_inode)
{
	lockdep_set_subclass(&ea_inode->i_rwsem, 1);
}