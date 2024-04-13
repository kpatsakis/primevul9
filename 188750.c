struct i40e_mac_filter *i40e_add_filter(struct i40e_vsi *vsi,
					const u8 *macaddr, s16 vlan)
{
	struct i40e_mac_filter *f;
	u64 key;

	if (!vsi || !macaddr)
		return NULL;

	f = i40e_find_filter(vsi, macaddr, vlan);
	if (!f) {
		f = kzalloc(sizeof(*f), GFP_ATOMIC);
		if (!f)
			return NULL;

		/* Update the boolean indicating if we need to function in
		 * VLAN mode.
		 */
		if (vlan >= 0)
			vsi->has_vlan_filter = true;

		ether_addr_copy(f->macaddr, macaddr);
		f->vlan = vlan;
		f->state = I40E_FILTER_NEW;
		INIT_HLIST_NODE(&f->hlist);

		key = i40e_addr_to_hkey(macaddr);
		hash_add(vsi->mac_filter_hash, &f->hlist, key);

		vsi->flags |= I40E_VSI_FLAG_FILTER_CHANGED;
		set_bit(__I40E_MACVLAN_SYNC_PENDING, vsi->back->state);
	}

	/* If we're asked to add a filter that has been marked for removal, it
	 * is safe to simply restore it to active state. __i40e_del_filter
	 * will have simply deleted any filters which were previously marked
	 * NEW or FAILED, so if it is currently marked REMOVE it must have
	 * previously been ACTIVE. Since we haven't yet run the sync filters
	 * task, just restore this filter to the ACTIVE state so that the
	 * sync task leaves it in place
	 */
	if (f->state == I40E_FILTER_REMOVE)
		f->state = I40E_FILTER_ACTIVE;

	return f;
}