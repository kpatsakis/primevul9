static void i40e_enable_mc_magic_wake(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	i40e_status ret;
	u8 mac_addr[6];
	u16 flags = 0;

	/* Get current MAC address in case it's an LAA */
	if (pf->vsi[pf->lan_vsi] && pf->vsi[pf->lan_vsi]->netdev) {
		ether_addr_copy(mac_addr,
				pf->vsi[pf->lan_vsi]->netdev->dev_addr);
	} else {
		dev_err(&pf->pdev->dev,
			"Failed to retrieve MAC address; using default\n");
		ether_addr_copy(mac_addr, hw->mac.addr);
	}

	/* The FW expects the mac address write cmd to first be called with
	 * one of these flags before calling it again with the multicast
	 * enable flags.
	 */
	flags = I40E_AQC_WRITE_TYPE_LAA_WOL;

	if (hw->func_caps.flex10_enable && hw->partition_id != 1)
		flags = I40E_AQC_WRITE_TYPE_LAA_ONLY;

	ret = i40e_aq_mac_address_write(hw, flags, mac_addr, NULL);
	if (ret) {
		dev_err(&pf->pdev->dev,
			"Failed to update MAC address registers; cannot enable Multicast Magic packet wake up");
		return;
	}

	flags = I40E_AQC_MC_MAG_EN
			| I40E_AQC_WOL_PRESERVE_ON_PFR
			| I40E_AQC_WRITE_TYPE_UPDATE_MC_MAG;
	ret = i40e_aq_mac_address_write(hw, flags, mac_addr, NULL);
	if (ret)
		dev_err(&pf->pdev->dev,
			"Failed to enable Multicast Magic Packet wake up\n");
}