struct sdma_engine *sdma_select_engine_vl(
	struct hfi1_devdata *dd,
	u32 selector,
	u8 vl)
{
	struct sdma_vl_map *m;
	struct sdma_map_elem *e;
	struct sdma_engine *rval;

	/* NOTE This should only happen if SC->VL changed after the initial
	 *      checks on the QP/AH
	 *      Default will return engine 0 below
	 */
	if (vl >= num_vls) {
		rval = NULL;
		goto done;
	}

	rcu_read_lock();
	m = rcu_dereference(dd->sdma_map);
	if (unlikely(!m)) {
		rcu_read_unlock();
		return &dd->per_sdma[0];
	}
	e = m->map[vl & m->mask];
	rval = e->sde[selector & e->mask];
	rcu_read_unlock();

done:
	rval =  !rval ? &dd->per_sdma[0] : rval;
	trace_hfi1_sdma_engine_select(dd, selector, vl, rval->this_idx);
	return rval;
}