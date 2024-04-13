static void i40e_undo_del_filter_entries(struct i40e_vsi *vsi,
					 struct hlist_head *from)
{
	struct i40e_mac_filter *f;
	struct hlist_node *h;

	hlist_for_each_entry_safe(f, h, from, hlist) {
		u64 key = i40e_addr_to_hkey(f->macaddr);

		/* Move the element back into MAC filter list*/
		hlist_del(&f->hlist);
		hash_add(vsi->mac_filter_hash, &f->hlist, key);
	}
}