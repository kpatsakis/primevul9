static void nfs4_add_and_init_slots(struct nfs4_slot_table *tbl,
		struct nfs4_slot *new,
		u32 max_slots,
		u32 ivalue)
{
	struct nfs4_slot *old = NULL;
	u32 i;

	spin_lock(&tbl->slot_tbl_lock);
	if (new) {
		old = tbl->slots;
		tbl->slots = new;
		tbl->max_slots = max_slots;
	}
	tbl->highest_used_slotid = -1;	/* no slot is currently used */
	for (i = 0; i < tbl->max_slots; i++)
		tbl->slots[i].seq_nr = ivalue;
	spin_unlock(&tbl->slot_tbl_lock);
	kfree(old);
}