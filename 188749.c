static void i40e_set_rx_mode(struct net_device *netdev)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;

	spin_lock_bh(&vsi->mac_filter_hash_lock);

	__dev_uc_sync(netdev, i40e_addr_sync, i40e_addr_unsync);
	__dev_mc_sync(netdev, i40e_addr_sync, i40e_addr_unsync);

	spin_unlock_bh(&vsi->mac_filter_hash_lock);

	/* check for other flag changes */
	if (vsi->current_netdev_flags != vsi->netdev->flags) {
		vsi->flags |= I40E_VSI_FLAG_FILTER_CHANGED;
		set_bit(__I40E_MACVLAN_SYNC_PENDING, vsi->back->state);
	}
}