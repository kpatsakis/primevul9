struct i40e_mac_filter *i40e_find_mac(struct i40e_vsi *vsi, const u8 *macaddr)
{
	struct i40e_mac_filter *f;
	u64 key;

	if (!vsi || !macaddr)
		return NULL;

	key = i40e_addr_to_hkey(macaddr);
	hash_for_each_possible(vsi->mac_filter_hash, f, hlist, key) {
		if ((ether_addr_equal(macaddr, f->macaddr)))
			return f;
	}
	return NULL;
}