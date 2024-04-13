int i40e_add_vlan_all_mac(struct i40e_vsi *vsi, s16 vid)
{
	struct i40e_mac_filter *f, *add_f;
	struct hlist_node *h;
	int bkt;

	hash_for_each_safe(vsi->mac_filter_hash, bkt, h, f, hlist) {
		if (f->state == I40E_FILTER_REMOVE)
			continue;
		add_f = i40e_add_filter(vsi, f->macaddr, vid);
		if (!add_f) {
			dev_info(&vsi->back->pdev->dev,
				 "Could not add vlan filter %d for %pM\n",
				 vid, f->macaddr);
			return -ENOMEM;
		}
	}

	return 0;
}