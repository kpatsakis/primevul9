static void ext4_xattr_inode_set_hash(struct inode *ea_inode, u32 hash)
{
	ea_inode->i_atime.tv_sec = hash;
}