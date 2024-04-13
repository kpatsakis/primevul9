static void init_once(void *foo)
{
	struct f2fs_inode_info *fi = (struct f2fs_inode_info *) foo;

	inode_init_once(&fi->vfs_inode);
}