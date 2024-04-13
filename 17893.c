size_t get_default_max_mem_usage(void)
{
	size_t total_mem_size = jas_get_total_mem_size();
	size_t max_mem;
	if (total_mem_size) {
		max_mem = 0.90 * total_mem_size;
	} else {
		max_mem = JAS_DEFAULT_MAX_MEM_USAGE;
	}
	return max_mem;
}