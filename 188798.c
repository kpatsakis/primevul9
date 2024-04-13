static int i40e_set_mac(struct net_device *netdev, void *p)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	if (ether_addr_equal(netdev->dev_addr, addr->sa_data)) {
		netdev_info(netdev, "already using mac address %pM\n",
			    addr->sa_data);
		return 0;
	}

	if (test_bit(__I40E_DOWN, pf->state) ||
	    test_bit(__I40E_RESET_RECOVERY_PENDING, pf->state))
		return -EADDRNOTAVAIL;

	if (ether_addr_equal(hw->mac.addr, addr->sa_data))
		netdev_info(netdev, "returning to hw mac address %pM\n",
			    hw->mac.addr);
	else
		netdev_info(netdev, "set new mac address %pM\n", addr->sa_data);

	/* Copy the address first, so that we avoid a possible race with
	 * .set_rx_mode().
	 * - Remove old address from MAC filter
	 * - Copy new address
	 * - Add new address to MAC filter
	 */
	spin_lock_bh(&vsi->mac_filter_hash_lock);
	i40e_del_mac_filter(vsi, netdev->dev_addr);
	ether_addr_copy(netdev->dev_addr, addr->sa_data);
	i40e_add_mac_filter(vsi, netdev->dev_addr);
	spin_unlock_bh(&vsi->mac_filter_hash_lock);

	if (vsi->type == I40E_VSI_MAIN) {
		i40e_status ret;

		ret = i40e_aq_mac_address_write(hw, I40E_AQC_WRITE_TYPE_LAA_WOL,
						addr->sa_data, NULL);
		if (ret)
			netdev_info(netdev, "Ignoring error from firmware on LAA update, status %s, AQ ret %s\n",
				    i40e_stat_str(hw, ret),
				    i40e_aq_str(hw, hw->aq.asq_last_status));
	}

	/* schedule our worker thread which will take care of
	 * applying the new filter changes
	 */
	i40e_service_event_schedule(pf);
	return 0;
}