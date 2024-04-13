static void ext4_xattr_inode_set_ref(struct inode *ea_inode, u64 ref_count)
{
	ea_inode->i_ctime.tv_sec = (u32)(ref_count >> 32);
	inode_set_iversion_raw(ea_inode, ref_count & 0xffffffff);
}