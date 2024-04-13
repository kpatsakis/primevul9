void fs_reclaim_acquire(gfp_t gfp_mask)
{
	if (__need_fs_reclaim(gfp_mask))
		lock_map_acquire(&__fs_reclaim_map);
}