static int me_swapcache_clean(struct page *p, unsigned long pfn)
{
	delete_from_swap_cache(p);

	if (!delete_from_lru_cache(p))
		return MF_RECOVERED;
	else
		return MF_FAILED;
}