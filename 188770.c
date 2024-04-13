static void i40e_undo_add_filter_entries(struct i40e_vsi *vsi,
					 struct hlist_head *from)
{
	struct i40e_new_mac_filter *new;
	struct hlist_node *h;

	hlist_for_each_entry_safe(new, h, from, hlist) {
		/* We can simply free the wrapper structure */
		hlist_del(&new->hlist);
		kfree(new);
	}
}