static int i40e_vlan_rx_kill_vid(struct net_device *netdev,
				 __always_unused __be16 proto, u16 vid)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;

	/* return code is ignored as there is nothing a user
	 * can do about failure to remove and a log message was
	 * already printed from the other function
	 */
	i40e_vsi_kill_vlan(vsi, vid);

	clear_bit(vid, vsi->active_vlans);

	return 0;
}