struct i40e_mac_filter *i40e_add_mac_filter(struct i40e_vsi *vsi,
					    const u8 *macaddr)
{
	struct i40e_mac_filter *f, *add = NULL;
	struct hlist_node *h;
	int bkt;

	if (vsi->info.pvid)
		return i40e_add_filter(vsi, macaddr,
				       le16_to_cpu(vsi->info.pvid));

	if (!i40e_is_vsi_in_vlan(vsi))
		return i40e_add_filter(vsi, macaddr, I40E_VLAN_ANY);

	hash_for_each_safe(vsi->mac_filter_hash, bkt, h, f, hlist) {
		if (f->state == I40E_FILTER_REMOVE)
			continue;
		add = i40e_add_filter(vsi, macaddr, f->vlan);
		if (!add)
			return NULL;
	}

	return add;
}