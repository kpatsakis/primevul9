static int ext4_xattr_inode_inc_ref(handle_t *handle, struct inode *ea_inode)
{
	return ext4_xattr_inode_update_ref(handle, ea_inode, 1);
}