static int nfs4_realloc_slot_table(struct nfs4_slot_table *tbl, u32 max_reqs,
				 u32 ivalue)
{
	struct nfs4_slot *new = NULL;
	int ret = -ENOMEM;

	dprintk("--> %s: max_reqs=%u, tbl->max_slots %d\n", __func__,
		max_reqs, tbl->max_slots);

	/* Does the newly negotiated max_reqs match the existing slot table? */
	if (max_reqs != tbl->max_slots) {
		new = nfs4_alloc_slots(max_reqs, GFP_NOFS);
		if (!new)
			goto out;
	}
	ret = 0;

	nfs4_add_and_init_slots(tbl, new, max_reqs, ivalue);
	dprintk("%s: tbl=%p slots=%p max_slots=%d\n", __func__,
		tbl, tbl->slots, tbl->max_slots);
out:
	dprintk("<-- %s: return %d\n", __func__, ret);
	return ret;
}