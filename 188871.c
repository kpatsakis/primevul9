void i40e_rm_vlan_all_mac(struct i40e_vsi *vsi, s16 vid)
{
	struct i40e_mac_filter *f;
	struct hlist_node *h;
	int bkt;

	hash_for_each_safe(vsi->mac_filter_hash, bkt, h, f, hlist) {
		if (f->vlan == vid)
			__i40e_del_filter(vsi, f);
	}
}