static inline unsigned long __init __def_free(unsigned long *nr_free,
					      unsigned long *free_base_pfn,
					      struct page **page)
{
	unsigned long nr = *nr_free;

	deferred_free_range(*free_base_pfn, nr);
	*free_base_pfn = 0;
	*nr_free = 0;
	*page = NULL;

	return nr;
}