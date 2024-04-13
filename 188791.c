int i40e_open(struct net_device *netdev)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	int err;

	/* disallow open during test or if eeprom is broken */
	if (test_bit(__I40E_TESTING, pf->state) ||
	    test_bit(__I40E_BAD_EEPROM, pf->state))
		return -EBUSY;

	netif_carrier_off(netdev);

	if (i40e_force_link_state(pf, true))
		return -EAGAIN;

	err = i40e_vsi_open(vsi);
	if (err)
		return err;

	/* configure global TSO hardware offload settings */
	wr32(&pf->hw, I40E_GLLAN_TSOMSK_F, be32_to_cpu(TCP_FLAG_PSH |
						       TCP_FLAG_FIN) >> 16);
	wr32(&pf->hw, I40E_GLLAN_TSOMSK_M, be32_to_cpu(TCP_FLAG_PSH |
						       TCP_FLAG_FIN |
						       TCP_FLAG_CWR) >> 16);
	wr32(&pf->hw, I40E_GLLAN_TSOMSK_L, be32_to_cpu(TCP_FLAG_CWR) >> 16);

	udp_tunnel_get_rx_info(netdev);

	return 0;
}