static int init_inodecache(void)
{
	ext4_inode_cachep = kmem_cache_create("ext4_inode_cache",
					     sizeof(struct ext4_inode_info),
					     0, (SLAB_RECLAIM_ACCOUNT|
						SLAB_MEM_SPREAD),
					     init_once);
	if (ext4_inode_cachep == NULL)
		return -ENOMEM;
	return 0;
}