int i40e_del_mac_filter(struct i40e_vsi *vsi, const u8 *macaddr)
{
	struct i40e_mac_filter *f;
	struct hlist_node *h;
	bool found = false;
	int bkt;

	lockdep_assert_held(&vsi->mac_filter_hash_lock);
	hash_for_each_safe(vsi->mac_filter_hash, bkt, h, f, hlist) {
		if (ether_addr_equal(macaddr, f->macaddr)) {
			__i40e_del_filter(vsi, f);
			found = true;
		}
	}

	if (found)
		return 0;
	else
		return -ENOENT;
}