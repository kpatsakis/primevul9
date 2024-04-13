static inline void f2fs_i_xnid_write(struct inode *inode, nid_t xnid)
{
	F2FS_I(inode)->i_xattr_nid = xnid;
	f2fs_mark_inode_dirty_sync(inode, true);
}