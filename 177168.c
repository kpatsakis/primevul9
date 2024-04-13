static u64 ext4_xattr_inode_get_ref(struct inode *ea_inode)
{
	return ((u64)ea_inode->i_ctime.tv_sec << 32) |
		(u32) inode_peek_iversion_raw(ea_inode);
}