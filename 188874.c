static int i40e_change_mtu(struct net_device *netdev, int new_mtu)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;

	if (i40e_enabled_xdp_vsi(vsi)) {
		int frame_size = new_mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN;

		if (frame_size > i40e_max_xdp_frame_size(vsi))
			return -EINVAL;
	}

	netdev_info(netdev, "changing MTU from %d to %d\n",
		    netdev->mtu, new_mtu);
	netdev->mtu = new_mtu;
	if (netif_running(netdev))
		i40e_vsi_reinit_locked(vsi);
	set_bit(__I40E_CLIENT_SERVICE_REQUESTED, pf->state);
	set_bit(__I40E_CLIENT_L2_CHANGE, pf->state);
	return 0;
}