bool i40e_is_vsi_in_vlan(struct i40e_vsi *vsi)
{
	/* If we have a PVID, always operate in VLAN mode */
	if (vsi->info.pvid)
		return true;

	/* We need to operate in VLAN mode whenever we have any filters with
	 * a VLAN other than I40E_VLAN_ALL. We could check the table each
	 * time, incurring search cost repeatedly. However, we can notice two
	 * things:
	 *
	 * 1) the only place where we can gain a VLAN filter is in
	 *    i40e_add_filter.
	 *
	 * 2) the only place where filters are actually removed is in
	 *    i40e_sync_filters_subtask.
	 *
	 * Thus, we can simply use a boolean value, has_vlan_filters which we
	 * will set to true when we add a VLAN filter in i40e_add_filter. Then
	 * we have to perform the full search after deleting filters in
	 * i40e_sync_filters_subtask, but we already have to search
	 * filters here and can perform the check at the same time. This
	 * results in avoiding embedding a loop for VLAN mode inside another
	 * loop over all the filters, and should maintain correctness as noted
	 * above.
	 */
	return vsi->has_vlan_filter;
}