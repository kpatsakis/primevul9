int sdma_engine_get_vl(struct sdma_engine *sde)
{
	struct hfi1_devdata *dd = sde->dd;
	struct sdma_vl_map *m;
	u8 vl;

	if (sde->this_idx >= TXE_NUM_SDMA_ENGINES)
		return -EINVAL;

	rcu_read_lock();
	m = rcu_dereference(dd->sdma_map);
	if (unlikely(!m)) {
		rcu_read_unlock();
		return -EINVAL;
	}
	vl = m->engine_to_vl[sde->this_idx];
	rcu_read_unlock();

	return vl;
}