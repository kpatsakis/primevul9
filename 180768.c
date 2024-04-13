static inline void zone_statistics(struct zone *preferred_zone, struct zone *z)
{
#ifdef CONFIG_NUMA
	enum numa_stat_item local_stat = NUMA_LOCAL;

	/* skip numa counters update if numa stats is disabled */
	if (!static_branch_likely(&vm_numa_stat_key))
		return;

	if (z->node != numa_node_id())
		local_stat = NUMA_OTHER;

	if (z->node == preferred_zone->node)
		__inc_numa_state(z, NUMA_HIT);
	else {
		__inc_numa_state(z, NUMA_MISS);
		__inc_numa_state(preferred_zone, NUMA_FOREIGN);
	}
	__inc_numa_state(z, local_stat);
#endif
}