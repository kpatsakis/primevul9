static u32 ext4_xattr_inode_get_hash(struct inode *ea_inode)
{
	return (u32)ea_inode->i_atime.tv_sec;
}