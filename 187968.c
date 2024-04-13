static inline char *alloc_one_pg_vec_page(unsigned long order)
{
	char *buffer = NULL;
	gfp_t gfp_flags = GFP_KERNEL | __GFP_COMP |
			  __GFP_ZERO | __GFP_NOWARN | __GFP_NORETRY;

	buffer = (char *) __get_free_pages(gfp_flags, order);

	if (buffer)
		return buffer;

	/*
	 * __get_free_pages failed, fall back to vmalloc
	 */
	buffer = vzalloc((1 << order) * PAGE_SIZE);

	if (buffer)
		return buffer;

	/*
	 * vmalloc failed, lets dig into swap here
	 */
	gfp_flags &= ~__GFP_NORETRY;
	buffer = (char *)__get_free_pages(gfp_flags, order);
	if (buffer)
		return buffer;

	/*
	 * complete and utter failure
	 */
	return NULL;
}