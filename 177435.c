static int me_swapcache_dirty(struct page *p, unsigned long pfn)
{
	ClearPageDirty(p);
	/* Trigger EIO in shmem: */
	ClearPageUptodate(p);

	if (!delete_from_lru_cache(p))
		return MF_DELAYED;
	else
		return MF_FAILED;
}