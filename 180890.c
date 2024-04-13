static bool show_mem_node_skip(unsigned int flags, int nid, nodemask_t *nodemask)
{
	if (!(flags & SHOW_MEM_FILTER_NODES))
		return false;

	/*
	 * no node mask - aka implicit memory numa policy. Do not bother with
	 * the synchronization - read_mems_allowed_begin - because we do not
	 * have to be precise here.
	 */
	if (!nodemask)
		nodemask = &cpuset_current_mems_allowed;

	return !node_isset(nid, *nodemask);
}