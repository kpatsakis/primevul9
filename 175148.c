static void *__kmalloc_reserve(size_t size, gfp_t flags, int node,
			       unsigned long ip, bool *pfmemalloc)
{
	void *obj;
	bool ret_pfmemalloc = false;

	/*
	 * Try a regular allocation, when that fails and we're not entitled
	 * to the reserves, fail.
	 */
	obj = kmalloc_node_track_caller(size,
					flags | __GFP_NOMEMALLOC | __GFP_NOWARN,
					node);
	if (obj || !(gfp_pfmemalloc_allowed(flags)))
		goto out;

	/* Try again but now we are using pfmemalloc reserves */
	ret_pfmemalloc = true;
	obj = kmalloc_node_track_caller(size, flags, node);

out:
	if (pfmemalloc)
		*pfmemalloc = ret_pfmemalloc;

	return obj;
}