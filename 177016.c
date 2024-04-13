struct btrfs_path *btrfs_alloc_path(void)
{
	return kmem_cache_zalloc(btrfs_path_cachep, GFP_NOFS);
}