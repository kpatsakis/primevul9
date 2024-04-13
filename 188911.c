static void i40e_rebuild(struct i40e_pf *pf, bool reinit, bool lock_acquired)
{
	int old_recovery_mode_bit = test_bit(__I40E_RECOVERY_MODE, pf->state);
	struct i40e_vsi *vsi = pf->vsi[pf->lan_vsi];
	struct i40e_hw *hw = &pf->hw;
	u8 set_fc_aq_fail = 0;
	i40e_status ret;
	u32 val;
	int v;

	if (test_bit(__I40E_EMP_RESET_INTR_RECEIVED, pf->state) &&
	    i40e_check_recovery_mode(pf)) {
		i40e_set_ethtool_ops(pf->vsi[pf->lan_vsi]->netdev);
	}

	if (test_bit(__I40E_DOWN, pf->state) &&
	    !test_bit(__I40E_RECOVERY_MODE, pf->state) &&
	    !old_recovery_mode_bit)
		goto clear_recovery;
	dev_dbg(&pf->pdev->dev, "Rebuilding internal switch\n");

	/* rebuild the basics for the AdminQ, HMC, and initial HW switch */
	ret = i40e_init_adminq(&pf->hw);
	if (ret) {
		dev_info(&pf->pdev->dev, "Rebuild AdminQ failed, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
		goto clear_recovery;
	}
	i40e_get_oem_version(&pf->hw);

	if (test_bit(__I40E_EMP_RESET_INTR_RECEIVED, pf->state) &&
	    ((hw->aq.fw_maj_ver == 4 && hw->aq.fw_min_ver <= 33) ||
	     hw->aq.fw_maj_ver < 4) && hw->mac.type == I40E_MAC_XL710) {
		/* The following delay is necessary for 4.33 firmware and older
		 * to recover after EMP reset. 200 ms should suffice but we
		 * put here 300 ms to be sure that FW is ready to operate
		 * after reset.
		 */
		mdelay(300);
	}

	/* re-verify the eeprom if we just had an EMP reset */
	if (test_and_clear_bit(__I40E_EMP_RESET_INTR_RECEIVED, pf->state))
		i40e_verify_eeprom(pf);

	/* if we are going out of or into recovery mode we have to act
	 * accordingly with regard to resources initialization
	 * and deinitialization
	 */
	if (test_bit(__I40E_RECOVERY_MODE, pf->state) ||
	    old_recovery_mode_bit) {
		if (i40e_get_capabilities(pf,
					  i40e_aqc_opc_list_func_capabilities))
			goto end_unlock;

		if (test_bit(__I40E_RECOVERY_MODE, pf->state)) {
			/* we're staying in recovery mode so we'll reinitialize
			 * misc vector here
			 */
			if (i40e_setup_misc_vector_for_recovery_mode(pf))
				goto end_unlock;
		} else {
			if (!lock_acquired)
				rtnl_lock();
			/* we're going out of recovery mode so we'll free
			 * the IRQ allocated specifically for recovery mode
			 * and restore the interrupt scheme
			 */
			free_irq(pf->pdev->irq, pf);
			i40e_clear_interrupt_scheme(pf);
			if (i40e_restore_interrupt_scheme(pf))
				goto end_unlock;
		}

		/* tell the firmware that we're starting */
		i40e_send_version(pf);

		/* bail out in case recovery mode was detected, as there is
		 * no need for further configuration.
		 */
		goto end_unlock;
	}

	i40e_clear_pxe_mode(hw);
	ret = i40e_get_capabilities(pf, i40e_aqc_opc_list_func_capabilities);
	if (ret)
		goto end_core_reset;

	ret = i40e_init_lan_hmc(hw, hw->func_caps.num_tx_qp,
				hw->func_caps.num_rx_qp, 0, 0);
	if (ret) {
		dev_info(&pf->pdev->dev, "init_lan_hmc failed: %d\n", ret);
		goto end_core_reset;
	}
	ret = i40e_configure_lan_hmc(hw, I40E_HMC_MODEL_DIRECT_ONLY);
	if (ret) {
		dev_info(&pf->pdev->dev, "configure_lan_hmc failed: %d\n", ret);
		goto end_core_reset;
	}

	/* Enable FW to write a default DCB config on link-up */
	i40e_aq_set_dcb_parameters(hw, true, NULL);

#ifdef CONFIG_I40E_DCB
	ret = i40e_init_pf_dcb(pf);
	if (ret) {
		dev_info(&pf->pdev->dev, "DCB init failed %d, disabled\n", ret);
		pf->flags &= ~I40E_FLAG_DCB_CAPABLE;
		/* Continue without DCB enabled */
	}
#endif /* CONFIG_I40E_DCB */
	/* do basic switch setup */
	if (!lock_acquired)
		rtnl_lock();
	ret = i40e_setup_pf_switch(pf, reinit);
	if (ret)
		goto end_unlock;

	/* The driver only wants link up/down and module qualification
	 * reports from firmware.  Note the negative logic.
	 */
	ret = i40e_aq_set_phy_int_mask(&pf->hw,
				       ~(I40E_AQ_EVENT_LINK_UPDOWN |
					 I40E_AQ_EVENT_MEDIA_NA |
					 I40E_AQ_EVENT_MODULE_QUAL_FAIL), NULL);
	if (ret)
		dev_info(&pf->pdev->dev, "set phy mask fail, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));

	/* make sure our flow control settings are restored */
	ret = i40e_set_fc(&pf->hw, &set_fc_aq_fail, true);
	if (ret)
		dev_dbg(&pf->pdev->dev, "setting flow control: ret = %s last_status = %s\n",
			i40e_stat_str(&pf->hw, ret),
			i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));

	/* Rebuild the VSIs and VEBs that existed before reset.
	 * They are still in our local switch element arrays, so only
	 * need to rebuild the switch model in the HW.
	 *
	 * If there were VEBs but the reconstitution failed, we'll try
	 * try to recover minimal use by getting the basic PF VSI working.
	 */
	if (vsi->uplink_seid != pf->mac_seid) {
		dev_dbg(&pf->pdev->dev, "attempting to rebuild switch\n");
		/* find the one VEB connected to the MAC, and find orphans */
		for (v = 0; v < I40E_MAX_VEB; v++) {
			if (!pf->veb[v])
				continue;

			if (pf->veb[v]->uplink_seid == pf->mac_seid ||
			    pf->veb[v]->uplink_seid == 0) {
				ret = i40e_reconstitute_veb(pf->veb[v]);

				if (!ret)
					continue;

				/* If Main VEB failed, we're in deep doodoo,
				 * so give up rebuilding the switch and set up
				 * for minimal rebuild of PF VSI.
				 * If orphan failed, we'll report the error
				 * but try to keep going.
				 */
				if (pf->veb[v]->uplink_seid == pf->mac_seid) {
					dev_info(&pf->pdev->dev,
						 "rebuild of switch failed: %d, will try to set up simple PF connection\n",
						 ret);
					vsi->uplink_seid = pf->mac_seid;
					break;
				} else if (pf->veb[v]->uplink_seid == 0) {
					dev_info(&pf->pdev->dev,
						 "rebuild of orphan VEB failed: %d\n",
						 ret);
				}
			}
		}
	}

	if (vsi->uplink_seid == pf->mac_seid) {
		dev_dbg(&pf->pdev->dev, "attempting to rebuild PF VSI\n");
		/* no VEB, so rebuild only the Main VSI */
		ret = i40e_add_vsi(vsi);
		if (ret) {
			dev_info(&pf->pdev->dev,
				 "rebuild of Main VSI failed: %d\n", ret);
			goto end_unlock;
		}
	}

	if (vsi->mqprio_qopt.max_rate[0]) {
		u64 max_tx_rate = vsi->mqprio_qopt.max_rate[0];
		u64 credits = 0;

		do_div(max_tx_rate, I40E_BW_MBPS_DIVISOR);
		ret = i40e_set_bw_limit(vsi, vsi->seid, max_tx_rate);
		if (ret)
			goto end_unlock;

		credits = max_tx_rate;
		do_div(credits, I40E_BW_CREDIT_DIVISOR);
		dev_dbg(&vsi->back->pdev->dev,
			"Set tx rate of %llu Mbps (count of 50Mbps %llu) for vsi->seid %u\n",
			max_tx_rate,
			credits,
			vsi->seid);
	}

	ret = i40e_rebuild_cloud_filters(vsi, vsi->seid);
	if (ret)
		goto end_unlock;

	/* PF Main VSI is rebuild by now, go ahead and rebuild channel VSIs
	 * for this main VSI if they exist
	 */
	ret = i40e_rebuild_channels(vsi);
	if (ret)
		goto end_unlock;

	/* Reconfigure hardware for allowing smaller MSS in the case
	 * of TSO, so that we avoid the MDD being fired and causing
	 * a reset in the case of small MSS+TSO.
	 */
#define I40E_REG_MSS          0x000E64DC
#define I40E_REG_MSS_MIN_MASK 0x3FF0000
#define I40E_64BYTE_MSS       0x400000
	val = rd32(hw, I40E_REG_MSS);
	if ((val & I40E_REG_MSS_MIN_MASK) > I40E_64BYTE_MSS) {
		val &= ~I40E_REG_MSS_MIN_MASK;
		val |= I40E_64BYTE_MSS;
		wr32(hw, I40E_REG_MSS, val);
	}

	if (pf->hw_features & I40E_HW_RESTART_AUTONEG) {
		msleep(75);
		ret = i40e_aq_set_link_restart_an(&pf->hw, true, NULL);
		if (ret)
			dev_info(&pf->pdev->dev, "link restart failed, err %s aq_err %s\n",
				 i40e_stat_str(&pf->hw, ret),
				 i40e_aq_str(&pf->hw,
					     pf->hw.aq.asq_last_status));
	}
	/* reinit the misc interrupt */
	if (pf->flags & I40E_FLAG_MSIX_ENABLED)
		ret = i40e_setup_misc_vector(pf);

	/* Add a filter to drop all Flow control frames from any VSI from being
	 * transmitted. By doing so we stop a malicious VF from sending out
	 * PAUSE or PFC frames and potentially controlling traffic for other
	 * PF/VF VSIs.
	 * The FW can still send Flow control frames if enabled.
	 */
	i40e_add_filter_to_drop_tx_flow_control_frames(&pf->hw,
						       pf->main_vsi_seid);

	/* restart the VSIs that were rebuilt and running before the reset */
	i40e_pf_unquiesce_all_vsi(pf);

	/* Release the RTNL lock before we start resetting VFs */
	if (!lock_acquired)
		rtnl_unlock();

	/* Restore promiscuous settings */
	ret = i40e_set_promiscuous(pf, pf->cur_promisc);
	if (ret)
		dev_warn(&pf->pdev->dev,
			 "Failed to restore promiscuous setting: %s, err %s aq_err %s\n",
			 pf->cur_promisc ? "on" : "off",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));

	i40e_reset_all_vfs(pf, true);

	/* tell the firmware that we're starting */
	i40e_send_version(pf);

	/* We've already released the lock, so don't do it again */
	goto end_core_reset;

end_unlock:
	if (!lock_acquired)
		rtnl_unlock();
end_core_reset:
	clear_bit(__I40E_RESET_FAILED, pf->state);
clear_recovery:
	clear_bit(__I40E_RESET_RECOVERY_PENDING, pf->state);
	clear_bit(__I40E_TIMEOUT_RECOVERY_PENDING, pf->state);
}