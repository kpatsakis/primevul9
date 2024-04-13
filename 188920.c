static int i40e_vlan_rx_add_vid(struct net_device *netdev,
				__always_unused __be16 proto, u16 vid)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	int ret = 0;

	if (vid >= VLAN_N_VID)
		return -EINVAL;

	ret = i40e_vsi_add_vlan(vsi, vid);
	if (!ret)
		set_bit(vid, vsi->active_vlans);

	return ret;
}