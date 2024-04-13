static int __init init_inodecache(void)
{
	f2fs_inode_cachep = f2fs_kmem_cache_create("f2fs_inode_cache",
			sizeof(struct f2fs_inode_info));
	if (!f2fs_inode_cachep)
		return -ENOMEM;
	return 0;
}