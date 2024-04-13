static inline bool update_defer_init(pg_data_t *pgdat,
				unsigned long pfn, unsigned long zone_end,
				unsigned long *nr_initialised)
{
	/* Always populate low zones for address-contrained allocations */
	if (zone_end < pgdat_end_pfn(pgdat))
		return true;
	(*nr_initialised)++;
	if ((*nr_initialised > pgdat->static_init_size) &&
	    (pfn & (PAGES_PER_SECTION - 1)) == 0) {
		pgdat->first_deferred_pfn = pfn;
		return false;
	}

	return true;
}