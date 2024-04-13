void i40e_vsi_kill_vlan(struct i40e_vsi *vsi, u16 vid)
{
	if (!vid || vsi->info.pvid)
		return;

	spin_lock_bh(&vsi->mac_filter_hash_lock);
	i40e_rm_vlan_all_mac(vsi, vid);
	spin_unlock_bh(&vsi->mac_filter_hash_lock);

	/* schedule our worker thread which will take care of
	 * applying the new filter changes
	 */
	i40e_service_event_schedule(vsi->back);
}