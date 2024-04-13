static void i40e_vlan_rx_add_vid_up(struct net_device *netdev,
				    __always_unused __be16 proto, u16 vid)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;

	if (vid >= VLAN_N_VID)
		return;
	set_bit(vid, vsi->active_vlans);
}