struct i40e_new_mac_filter *i40e_next_filter(struct i40e_new_mac_filter *next)
{
	hlist_for_each_entry_continue(next, hlist) {
		if (!is_broadcast_ether_addr(next->f->macaddr))
			return next;
	}

	return NULL;
}