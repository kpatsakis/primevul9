static int i40e_set_features(struct net_device *netdev,
			     netdev_features_t features)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	bool need_reset;

	if (features & NETIF_F_RXHASH && !(netdev->features & NETIF_F_RXHASH))
		i40e_pf_config_rss(pf);
	else if (!(features & NETIF_F_RXHASH) &&
		 netdev->features & NETIF_F_RXHASH)
		i40e_clear_rss_lut(vsi);

	if (features & NETIF_F_HW_VLAN_CTAG_RX)
		i40e_vlan_stripping_enable(vsi);
	else
		i40e_vlan_stripping_disable(vsi);

	if (!(features & NETIF_F_HW_TC) && pf->num_cloud_filters) {
		dev_err(&pf->pdev->dev,
			"Offloaded tc filters active, can't turn hw_tc_offload off");
		return -EINVAL;
	}

	if (!(features & NETIF_F_HW_L2FW_DOFFLOAD) && vsi->macvlan_cnt)
		i40e_del_all_macvlans(vsi);

	need_reset = i40e_set_ntuple(pf, features);

	if (need_reset)
		i40e_do_reset(pf, I40E_PF_RESET_FLAG, true);

	return 0;
}