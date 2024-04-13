static __always_inline struct page *__rmqueue_cma_fallback(struct zone *zone,
					unsigned int order)
{
	return __rmqueue_smallest(zone, order, MIGRATE_CMA);
}