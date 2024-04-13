static int i40e_correct_mac_vlan_filters(struct i40e_vsi *vsi,
					 struct hlist_head *tmp_add_list,
					 struct hlist_head *tmp_del_list,
					 int vlan_filters)
{
	s16 pvid = le16_to_cpu(vsi->info.pvid);
	struct i40e_mac_filter *f, *add_head;
	struct i40e_new_mac_filter *new;
	struct hlist_node *h;
	int bkt, new_vlan;

	/* To determine if a particular filter needs to be replaced we
	 * have the three following conditions:
	 *
	 * a) if we have a PVID assigned, then all filters which are
	 *    not marked as VLAN=PVID must be replaced with filters that
	 *    are.
	 * b) otherwise, if we have any active VLANS, all filters
	 *    which are marked as VLAN=-1 must be replaced with
	 *    filters marked as VLAN=0
	 * c) finally, if we do not have any active VLANS, all filters
	 *    which are marked as VLAN=0 must be replaced with filters
	 *    marked as VLAN=-1
	 */

	/* Update the filters about to be added in place */
	hlist_for_each_entry(new, tmp_add_list, hlist) {
		if (pvid && new->f->vlan != pvid)
			new->f->vlan = pvid;
		else if (vlan_filters && new->f->vlan == I40E_VLAN_ANY)
			new->f->vlan = 0;
		else if (!vlan_filters && new->f->vlan == 0)
			new->f->vlan = I40E_VLAN_ANY;
	}

	/* Update the remaining active filters */
	hash_for_each_safe(vsi->mac_filter_hash, bkt, h, f, hlist) {
		/* Combine the checks for whether a filter needs to be changed
		 * and then determine the new VLAN inside the if block, in
		 * order to avoid duplicating code for adding the new filter
		 * then deleting the old filter.
		 */
		if ((pvid && f->vlan != pvid) ||
		    (vlan_filters && f->vlan == I40E_VLAN_ANY) ||
		    (!vlan_filters && f->vlan == 0)) {
			/* Determine the new vlan we will be adding */
			if (pvid)
				new_vlan = pvid;
			else if (vlan_filters)
				new_vlan = 0;
			else
				new_vlan = I40E_VLAN_ANY;

			/* Create the new filter */
			add_head = i40e_add_filter(vsi, f->macaddr, new_vlan);
			if (!add_head)
				return -ENOMEM;

			/* Create a temporary i40e_new_mac_filter */
			new = kzalloc(sizeof(*new), GFP_ATOMIC);
			if (!new)
				return -ENOMEM;

			new->f = add_head;
			new->state = add_head->state;

			/* Add the new filter to the tmp list */
			hlist_add_head(&new->hlist, tmp_add_list);

			/* Put the original filter into the delete list */
			f->state = I40E_FILTER_REMOVE;
			hash_del(&f->hlist);
			hlist_add_head(&f->hlist, tmp_del_list);
		}
	}

	vsi->has_vlan_filter = !!vlan_filters;

	return 0;
}