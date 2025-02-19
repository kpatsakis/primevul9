struct sdma_engine *sdma_select_user_engine(struct hfi1_devdata *dd,
					    u32 selector, u8 vl)
{
	struct sdma_rht_node *rht_node;
	struct sdma_engine *sde = NULL;
	unsigned long cpu_id;

	/*
	 * To ensure that always the same sdma engine(s) will be
	 * selected make sure the process is pinned to this CPU only.
	 */
	if (current->nr_cpus_allowed != 1)
		goto out;

	cpu_id = smp_processor_id();
	rcu_read_lock();
	rht_node = rhashtable_lookup_fast(dd->sdma_rht, &cpu_id,
					  sdma_rht_params);

	if (rht_node && rht_node->map[vl]) {
		struct sdma_rht_map_elem *map = rht_node->map[vl];

		sde = map->sde[selector & map->mask];
	}
	rcu_read_unlock();

	if (sde)
		return sde;

out:
	return sdma_select_engine_vl(dd, selector, vl);
}