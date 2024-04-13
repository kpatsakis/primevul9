void *snd_malloc_pages(size_t size, gfp_t gfp_flags)
{
	int pg;
	void *res;

	snd_assert(size > 0, return NULL);
	snd_assert(gfp_flags != 0, return NULL);
	gfp_flags |= __GFP_COMP;	/* compound page lets parts be mapped */
	pg = get_order(size);
	if ((res = (void *) __get_free_pages(gfp_flags, pg)) != NULL)
		inc_snd_pages(pg);
	return res;
}