nfs4_free_slot(struct nfs4_slot_table *tbl, u32 slotid)
{
	BUG_ON(slotid >= NFS4_MAX_SLOT_TABLE);
	/* clear used bit in bitmap */
	__clear_bit(slotid, tbl->used_slots);

	/* update highest_used_slotid when it is freed */
	if (slotid == tbl->highest_used_slotid) {
		slotid = find_last_bit(tbl->used_slots, tbl->max_slots);
		if (slotid < tbl->max_slots)
			tbl->highest_used_slotid = slotid;
		else
			tbl->highest_used_slotid = NFS4_NO_SLOT;
	}
	dprintk("%s: slotid %u highest_used_slotid %d\n", __func__,
		slotid, tbl->highest_used_slotid);
}