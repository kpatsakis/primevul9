static int i40e_addr_sync(struct net_device *netdev, const u8 *addr)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;

	if (i40e_add_mac_filter(vsi, addr))
		return 0;
	else
		return -ENOMEM;
}