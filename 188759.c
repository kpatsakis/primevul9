static int i40e_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct i40e_aq_get_phy_abilities_resp abilities;
	struct i40e_pf *pf;
	struct i40e_hw *hw;
	static u16 pfs_found;
	u16 wol_nvm_bits;
	u16 link_status;
	int err;
	u32 val;
	u32 i;
	u8 set_fc_aq_fail;

	err = pci_enable_device_mem(pdev);
	if (err)
		return err;

	/* set up for high or low dma */
	err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
	if (err) {
		err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
		if (err) {
			dev_err(&pdev->dev,
				"DMA configuration failed: 0x%x\n", err);
			goto err_dma;
		}
	}

	/* set up pci connections */
	err = pci_request_mem_regions(pdev, i40e_driver_name);
	if (err) {
		dev_info(&pdev->dev,
			 "pci_request_selected_regions failed %d\n", err);
		goto err_pci_reg;
	}

	pci_enable_pcie_error_reporting(pdev);
	pci_set_master(pdev);

	/* Now that we have a PCI connection, we need to do the
	 * low level device setup.  This is primarily setting up
	 * the Admin Queue structures and then querying for the
	 * device's current profile information.
	 */
	pf = kzalloc(sizeof(*pf), GFP_KERNEL);
	if (!pf) {
		err = -ENOMEM;
		goto err_pf_alloc;
	}
	pf->next_vsi = 0;
	pf->pdev = pdev;
	set_bit(__I40E_DOWN, pf->state);

	hw = &pf->hw;
	hw->back = pf;

	pf->ioremap_len = min_t(int, pci_resource_len(pdev, 0),
				I40E_MAX_CSR_SPACE);
	/* We believe that the highest register to read is
	 * I40E_GLGEN_STAT_CLEAR, so we check if the BAR size
	 * is not less than that before mapping to prevent a
	 * kernel panic.
	 */
	if (pf->ioremap_len < I40E_GLGEN_STAT_CLEAR) {
		dev_err(&pdev->dev, "Cannot map registers, bar size 0x%X too small, aborting\n",
			pf->ioremap_len);
		err = -ENOMEM;
		goto err_ioremap;
	}
	hw->hw_addr = ioremap(pci_resource_start(pdev, 0), pf->ioremap_len);
	if (!hw->hw_addr) {
		err = -EIO;
		dev_info(&pdev->dev, "ioremap(0x%04x, 0x%04x) failed: 0x%x\n",
			 (unsigned int)pci_resource_start(pdev, 0),
			 pf->ioremap_len, err);
		goto err_ioremap;
	}
	hw->vendor_id = pdev->vendor;
	hw->device_id = pdev->device;
	pci_read_config_byte(pdev, PCI_REVISION_ID, &hw->revision_id);
	hw->subsystem_vendor_id = pdev->subsystem_vendor;
	hw->subsystem_device_id = pdev->subsystem_device;
	hw->bus.device = PCI_SLOT(pdev->devfn);
	hw->bus.func = PCI_FUNC(pdev->devfn);
	hw->bus.bus_id = pdev->bus->number;
	pf->instance = pfs_found;

	/* Select something other than the 802.1ad ethertype for the
	 * switch to use internally and drop on ingress.
	 */
	hw->switch_tag = 0xffff;
	hw->first_tag = ETH_P_8021AD;
	hw->second_tag = ETH_P_8021Q;

	INIT_LIST_HEAD(&pf->l3_flex_pit_list);
	INIT_LIST_HEAD(&pf->l4_flex_pit_list);
	INIT_LIST_HEAD(&pf->ddp_old_prof);

	/* set up the locks for the AQ, do this only once in probe
	 * and destroy them only once in remove
	 */
	mutex_init(&hw->aq.asq_mutex);
	mutex_init(&hw->aq.arq_mutex);

	pf->msg_enable = netif_msg_init(debug,
					NETIF_MSG_DRV |
					NETIF_MSG_PROBE |
					NETIF_MSG_LINK);
	if (debug < -1)
		pf->hw.debug_mask = debug;

	/* do a special CORER for clearing PXE mode once at init */
	if (hw->revision_id == 0 &&
	    (rd32(hw, I40E_GLLAN_RCTL_0) & I40E_GLLAN_RCTL_0_PXE_MODE_MASK)) {
		wr32(hw, I40E_GLGEN_RTRIG, I40E_GLGEN_RTRIG_CORER_MASK);
		i40e_flush(hw);
		msleep(200);
		pf->corer_count++;

		i40e_clear_pxe_mode(hw);
	}

	/* Reset here to make sure all is clean and to define PF 'n' */
	i40e_clear_hw(hw);

	err = i40e_set_mac_type(hw);
	if (err) {
		dev_warn(&pdev->dev, "unidentified MAC or BLANK NVM: %d\n",
			 err);
		goto err_pf_reset;
	}

	err = i40e_pf_loop_reset(pf);
	if (err) {
		dev_info(&pdev->dev, "Initial pf_reset failed: %d\n", err);
		goto err_pf_reset;
	}

	i40e_check_recovery_mode(pf);

	hw->aq.num_arq_entries = I40E_AQ_LEN;
	hw->aq.num_asq_entries = I40E_AQ_LEN;
	hw->aq.arq_buf_size = I40E_MAX_AQ_BUF_SIZE;
	hw->aq.asq_buf_size = I40E_MAX_AQ_BUF_SIZE;
	pf->adminq_work_limit = I40E_AQ_WORK_LIMIT;

	snprintf(pf->int_name, sizeof(pf->int_name) - 1,
		 "%s-%s:misc",
		 dev_driver_string(&pf->pdev->dev), dev_name(&pdev->dev));

	err = i40e_init_shared_code(hw);
	if (err) {
		dev_warn(&pdev->dev, "unidentified MAC or BLANK NVM: %d\n",
			 err);
		goto err_pf_reset;
	}

	/* set up a default setting for link flow control */
	pf->hw.fc.requested_mode = I40E_FC_NONE;

	err = i40e_init_adminq(hw);
	if (err) {
		if (err == I40E_ERR_FIRMWARE_API_VERSION)
			dev_info(&pdev->dev,
				 "The driver for the device stopped because the NVM image v%u.%u is newer than expected v%u.%u. You must install the most recent version of the network driver.\n",
				 hw->aq.api_maj_ver,
				 hw->aq.api_min_ver,
				 I40E_FW_API_VERSION_MAJOR,
				 I40E_FW_MINOR_VERSION(hw));
		else
			dev_info(&pdev->dev,
				 "The driver for the device stopped because the device firmware failed to init. Try updating your NVM image.\n");

		goto err_pf_reset;
	}
	i40e_get_oem_version(hw);

	/* provide nvm, fw, api versions, vendor:device id, subsys vendor:device id */
	dev_info(&pdev->dev, "fw %d.%d.%05d api %d.%d nvm %s [%04x:%04x] [%04x:%04x]\n",
		 hw->aq.fw_maj_ver, hw->aq.fw_min_ver, hw->aq.fw_build,
		 hw->aq.api_maj_ver, hw->aq.api_min_ver,
		 i40e_nvm_version_str(hw), hw->vendor_id, hw->device_id,
		 hw->subsystem_vendor_id, hw->subsystem_device_id);

	if (hw->aq.api_maj_ver == I40E_FW_API_VERSION_MAJOR &&
	    hw->aq.api_min_ver > I40E_FW_MINOR_VERSION(hw))
		dev_info(&pdev->dev,
			 "The driver for the device detected a newer version of the NVM image v%u.%u than expected v%u.%u. Please install the most recent version of the network driver.\n",
			 hw->aq.api_maj_ver,
			 hw->aq.api_min_ver,
			 I40E_FW_API_VERSION_MAJOR,
			 I40E_FW_MINOR_VERSION(hw));
	else if (hw->aq.api_maj_ver == 1 && hw->aq.api_min_ver < 4)
		dev_info(&pdev->dev,
			 "The driver for the device detected an older version of the NVM image v%u.%u than expected v%u.%u. Please update the NVM image.\n",
			 hw->aq.api_maj_ver,
			 hw->aq.api_min_ver,
			 I40E_FW_API_VERSION_MAJOR,
			 I40E_FW_MINOR_VERSION(hw));

	i40e_verify_eeprom(pf);

	/* Rev 0 hardware was never productized */
	if (hw->revision_id < 1)
		dev_warn(&pdev->dev, "This device is a pre-production adapter/LOM. Please be aware there may be issues with your hardware. If you are experiencing problems please contact your Intel or hardware representative who provided you with this hardware.\n");

	i40e_clear_pxe_mode(hw);

	err = i40e_get_capabilities(pf, i40e_aqc_opc_list_func_capabilities);
	if (err)
		goto err_adminq_setup;

	err = i40e_sw_init(pf);
	if (err) {
		dev_info(&pdev->dev, "sw_init failed: %d\n", err);
		goto err_sw_init;
	}

	if (test_bit(__I40E_RECOVERY_MODE, pf->state))
		return i40e_init_recovery_mode(pf, hw);

	err = i40e_init_lan_hmc(hw, hw->func_caps.num_tx_qp,
				hw->func_caps.num_rx_qp, 0, 0);
	if (err) {
		dev_info(&pdev->dev, "init_lan_hmc failed: %d\n", err);
		goto err_init_lan_hmc;
	}

	err = i40e_configure_lan_hmc(hw, I40E_HMC_MODEL_DIRECT_ONLY);
	if (err) {
		dev_info(&pdev->dev, "configure_lan_hmc failed: %d\n", err);
		err = -ENOENT;
		goto err_configure_lan_hmc;
	}

	/* Disable LLDP for NICs that have firmware versions lower than v4.3.
	 * Ignore error return codes because if it was already disabled via
	 * hardware settings this will fail
	 */
	if (pf->hw_features & I40E_HW_STOP_FW_LLDP) {
		dev_info(&pdev->dev, "Stopping firmware LLDP agent.\n");
		i40e_aq_stop_lldp(hw, true, false, NULL);
	}

	/* allow a platform config to override the HW addr */
	i40e_get_platform_mac_addr(pdev, pf);

	if (!is_valid_ether_addr(hw->mac.addr)) {
		dev_info(&pdev->dev, "invalid MAC address %pM\n", hw->mac.addr);
		err = -EIO;
		goto err_mac_addr;
	}
	dev_info(&pdev->dev, "MAC address: %pM\n", hw->mac.addr);
	ether_addr_copy(hw->mac.perm_addr, hw->mac.addr);
	i40e_get_port_mac_addr(hw, hw->mac.port_addr);
	if (is_valid_ether_addr(hw->mac.port_addr))
		pf->hw_features |= I40E_HW_PORT_ID_VALID;

	pci_set_drvdata(pdev, pf);
	pci_save_state(pdev);

	dev_info(&pdev->dev,
		 (pf->flags & I40E_FLAG_DISABLE_FW_LLDP) ?
			"FW LLDP is disabled\n" :
			"FW LLDP is enabled\n");

	/* Enable FW to write default DCB config on link-up */
	i40e_aq_set_dcb_parameters(hw, true, NULL);

#ifdef CONFIG_I40E_DCB
	err = i40e_init_pf_dcb(pf);
	if (err) {
		dev_info(&pdev->dev, "DCB init failed %d, disabled\n", err);
		pf->flags &= ~(I40E_FLAG_DCB_CAPABLE | I40E_FLAG_DCB_ENABLED);
		/* Continue without DCB enabled */
	}
#endif /* CONFIG_I40E_DCB */

	/* set up periodic task facility */
	timer_setup(&pf->service_timer, i40e_service_timer, 0);
	pf->service_timer_period = HZ;

	INIT_WORK(&pf->service_task, i40e_service_task);
	clear_bit(__I40E_SERVICE_SCHED, pf->state);

	/* NVM bit on means WoL disabled for the port */
	i40e_read_nvm_word(hw, I40E_SR_NVM_WAKE_ON_LAN, &wol_nvm_bits);
	if (BIT (hw->port) & wol_nvm_bits || hw->partition_id != 1)
		pf->wol_en = false;
	else
		pf->wol_en = true;
	device_set_wakeup_enable(&pf->pdev->dev, pf->wol_en);

	/* set up the main switch operations */
	i40e_determine_queue_usage(pf);
	err = i40e_init_interrupt_scheme(pf);
	if (err)
		goto err_switch_setup;

	/* The number of VSIs reported by the FW is the minimum guaranteed
	 * to us; HW supports far more and we share the remaining pool with
	 * the other PFs. We allocate space for more than the guarantee with
	 * the understanding that we might not get them all later.
	 */
	if (pf->hw.func_caps.num_vsis < I40E_MIN_VSI_ALLOC)
		pf->num_alloc_vsi = I40E_MIN_VSI_ALLOC;
	else
		pf->num_alloc_vsi = pf->hw.func_caps.num_vsis;

	/* Set up the *vsi struct and our local tracking of the MAIN PF vsi. */
	pf->vsi = kcalloc(pf->num_alloc_vsi, sizeof(struct i40e_vsi *),
			  GFP_KERNEL);
	if (!pf->vsi) {
		err = -ENOMEM;
		goto err_switch_setup;
	}

#ifdef CONFIG_PCI_IOV
	/* prep for VF support */
	if ((pf->flags & I40E_FLAG_SRIOV_ENABLED) &&
	    (pf->flags & I40E_FLAG_MSIX_ENABLED) &&
	    !test_bit(__I40E_BAD_EEPROM, pf->state)) {
		if (pci_num_vf(pdev))
			pf->flags |= I40E_FLAG_VEB_MODE_ENABLED;
	}
#endif
	err = i40e_setup_pf_switch(pf, false);
	if (err) {
		dev_info(&pdev->dev, "setup_pf_switch failed: %d\n", err);
		goto err_vsis;
	}
	INIT_LIST_HEAD(&pf->vsi[pf->lan_vsi]->ch_list);

	/* Make sure flow control is set according to current settings */
	err = i40e_set_fc(hw, &set_fc_aq_fail, true);
	if (set_fc_aq_fail & I40E_SET_FC_AQ_FAIL_GET)
		dev_dbg(&pf->pdev->dev,
			"Set fc with err %s aq_err %s on get_phy_cap\n",
			i40e_stat_str(hw, err),
			i40e_aq_str(hw, hw->aq.asq_last_status));
	if (set_fc_aq_fail & I40E_SET_FC_AQ_FAIL_SET)
		dev_dbg(&pf->pdev->dev,
			"Set fc with err %s aq_err %s on set_phy_config\n",
			i40e_stat_str(hw, err),
			i40e_aq_str(hw, hw->aq.asq_last_status));
	if (set_fc_aq_fail & I40E_SET_FC_AQ_FAIL_UPDATE)
		dev_dbg(&pf->pdev->dev,
			"Set fc with err %s aq_err %s on get_link_info\n",
			i40e_stat_str(hw, err),
			i40e_aq_str(hw, hw->aq.asq_last_status));

	/* if FDIR VSI was set up, start it now */
	for (i = 0; i < pf->num_alloc_vsi; i++) {
		if (pf->vsi[i] && pf->vsi[i]->type == I40E_VSI_FDIR) {
			i40e_vsi_open(pf->vsi[i]);
			break;
		}
	}

	/* The driver only wants link up/down and module qualification
	 * reports from firmware.  Note the negative logic.
	 */
	err = i40e_aq_set_phy_int_mask(&pf->hw,
				       ~(I40E_AQ_EVENT_LINK_UPDOWN |
					 I40E_AQ_EVENT_MEDIA_NA |
					 I40E_AQ_EVENT_MODULE_QUAL_FAIL), NULL);
	if (err)
		dev_info(&pf->pdev->dev, "set phy mask fail, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, err),
			 i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));

	/* Reconfigure hardware for allowing smaller MSS in the case
	 * of TSO, so that we avoid the MDD being fired and causing
	 * a reset in the case of small MSS+TSO.
	 */
	val = rd32(hw, I40E_REG_MSS);
	if ((val & I40E_REG_MSS_MIN_MASK) > I40E_64BYTE_MSS) {
		val &= ~I40E_REG_MSS_MIN_MASK;
		val |= I40E_64BYTE_MSS;
		wr32(hw, I40E_REG_MSS, val);
	}

	if (pf->hw_features & I40E_HW_RESTART_AUTONEG) {
		msleep(75);
		err = i40e_aq_set_link_restart_an(&pf->hw, true, NULL);
		if (err)
			dev_info(&pf->pdev->dev, "link restart failed, err %s aq_err %s\n",
				 i40e_stat_str(&pf->hw, err),
				 i40e_aq_str(&pf->hw,
					     pf->hw.aq.asq_last_status));
	}
	/* The main driver is (mostly) up and happy. We need to set this state
	 * before setting up the misc vector or we get a race and the vector
	 * ends up disabled forever.
	 */
	clear_bit(__I40E_DOWN, pf->state);

	/* In case of MSIX we are going to setup the misc vector right here
	 * to handle admin queue events etc. In case of legacy and MSI
	 * the misc functionality and queue processing is combined in
	 * the same vector and that gets setup at open.
	 */
	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		err = i40e_setup_misc_vector(pf);
		if (err) {
			dev_info(&pdev->dev,
				 "setup of misc vector failed: %d\n", err);
			goto err_vsis;
		}
	}

#ifdef CONFIG_PCI_IOV
	/* prep for VF support */
	if ((pf->flags & I40E_FLAG_SRIOV_ENABLED) &&
	    (pf->flags & I40E_FLAG_MSIX_ENABLED) &&
	    !test_bit(__I40E_BAD_EEPROM, pf->state)) {
		/* disable link interrupts for VFs */
		val = rd32(hw, I40E_PFGEN_PORTMDIO_NUM);
		val &= ~I40E_PFGEN_PORTMDIO_NUM_VFLINK_STAT_ENA_MASK;
		wr32(hw, I40E_PFGEN_PORTMDIO_NUM, val);
		i40e_flush(hw);

		if (pci_num_vf(pdev)) {
			dev_info(&pdev->dev,
				 "Active VFs found, allocating resources.\n");
			err = i40e_alloc_vfs(pf, pci_num_vf(pdev));
			if (err)
				dev_info(&pdev->dev,
					 "Error %d allocating resources for existing VFs\n",
					 err);
		}
	}
#endif /* CONFIG_PCI_IOV */

	if (pf->flags & I40E_FLAG_IWARP_ENABLED) {
		pf->iwarp_base_vector = i40e_get_lump(pf, pf->irq_pile,
						      pf->num_iwarp_msix,
						      I40E_IWARP_IRQ_PILE_ID);
		if (pf->iwarp_base_vector < 0) {
			dev_info(&pdev->dev,
				 "failed to get tracking for %d vectors for IWARP err=%d\n",
				 pf->num_iwarp_msix, pf->iwarp_base_vector);
			pf->flags &= ~I40E_FLAG_IWARP_ENABLED;
		}
	}

	i40e_dbg_pf_init(pf);

	/* tell the firmware that we're starting */
	i40e_send_version(pf);

	/* since everything's happy, start the service_task timer */
	mod_timer(&pf->service_timer,
		  round_jiffies(jiffies + pf->service_timer_period));

	/* add this PF to client device list and launch a client service task */
	if (pf->flags & I40E_FLAG_IWARP_ENABLED) {
		err = i40e_lan_add_device(pf);
		if (err)
			dev_info(&pdev->dev, "Failed to add PF to client API service list: %d\n",
				 err);
	}

#define PCI_SPEED_SIZE 8
#define PCI_WIDTH_SIZE 8
	/* Devices on the IOSF bus do not have this information
	 * and will report PCI Gen 1 x 1 by default so don't bother
	 * checking them.
	 */
	if (!(pf->hw_features & I40E_HW_NO_PCI_LINK_CHECK)) {
		char speed[PCI_SPEED_SIZE] = "Unknown";
		char width[PCI_WIDTH_SIZE] = "Unknown";

		/* Get the negotiated link width and speed from PCI config
		 * space
		 */
		pcie_capability_read_word(pf->pdev, PCI_EXP_LNKSTA,
					  &link_status);

		i40e_set_pci_config_data(hw, link_status);

		switch (hw->bus.speed) {
		case i40e_bus_speed_8000:
			strlcpy(speed, "8.0", PCI_SPEED_SIZE); break;
		case i40e_bus_speed_5000:
			strlcpy(speed, "5.0", PCI_SPEED_SIZE); break;
		case i40e_bus_speed_2500:
			strlcpy(speed, "2.5", PCI_SPEED_SIZE); break;
		default:
			break;
		}
		switch (hw->bus.width) {
		case i40e_bus_width_pcie_x8:
			strlcpy(width, "8", PCI_WIDTH_SIZE); break;
		case i40e_bus_width_pcie_x4:
			strlcpy(width, "4", PCI_WIDTH_SIZE); break;
		case i40e_bus_width_pcie_x2:
			strlcpy(width, "2", PCI_WIDTH_SIZE); break;
		case i40e_bus_width_pcie_x1:
			strlcpy(width, "1", PCI_WIDTH_SIZE); break;
		default:
			break;
		}

		dev_info(&pdev->dev, "PCI-Express: Speed %sGT/s Width x%s\n",
			 speed, width);

		if (hw->bus.width < i40e_bus_width_pcie_x8 ||
		    hw->bus.speed < i40e_bus_speed_8000) {
			dev_warn(&pdev->dev, "PCI-Express bandwidth available for this device may be insufficient for optimal performance.\n");
			dev_warn(&pdev->dev, "Please move the device to a different PCI-e link with more lanes and/or higher transfer rate.\n");
		}
	}

	/* get the requested speeds from the fw */
	err = i40e_aq_get_phy_capabilities(hw, false, false, &abilities, NULL);
	if (err)
		dev_dbg(&pf->pdev->dev, "get requested speeds ret =  %s last_status =  %s\n",
			i40e_stat_str(&pf->hw, err),
			i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));
	pf->hw.phy.link_info.requested_speeds = abilities.link_speed;

	/* set the FEC config due to the board capabilities */
	i40e_set_fec_in_flags(abilities.fec_cfg_curr_mod_ext_info, &pf->flags);

	/* get the supported phy types from the fw */
	err = i40e_aq_get_phy_capabilities(hw, false, true, &abilities, NULL);
	if (err)
		dev_dbg(&pf->pdev->dev, "get supported phy types ret =  %s last_status =  %s\n",
			i40e_stat_str(&pf->hw, err),
			i40e_aq_str(&pf->hw, pf->hw.aq.asq_last_status));

	/* Add a filter to drop all Flow control frames from any VSI from being
	 * transmitted. By doing so we stop a malicious VF from sending out
	 * PAUSE or PFC frames and potentially controlling traffic for other
	 * PF/VF VSIs.
	 * The FW can still send Flow control frames if enabled.
	 */
	i40e_add_filter_to_drop_tx_flow_control_frames(&pf->hw,
						       pf->main_vsi_seid);

	if ((pf->hw.device_id == I40E_DEV_ID_10G_BASE_T) ||
		(pf->hw.device_id == I40E_DEV_ID_10G_BASE_T4))
		pf->hw_features |= I40E_HW_PHY_CONTROLS_LEDS;
	if (pf->hw.device_id == I40E_DEV_ID_SFP_I_X722)
		pf->hw_features |= I40E_HW_HAVE_CRT_RETIMER;
	/* print a string summarizing features */
	i40e_print_features(pf);

	return 0;

	/* Unwind what we've done if something failed in the setup */
err_vsis:
	set_bit(__I40E_DOWN, pf->state);
	i40e_clear_interrupt_scheme(pf);
	kfree(pf->vsi);
err_switch_setup:
	i40e_reset_interrupt_capability(pf);
	del_timer_sync(&pf->service_timer);
err_mac_addr:
err_configure_lan_hmc:
	(void)i40e_shutdown_lan_hmc(hw);
err_init_lan_hmc:
	kfree(pf->qp_pile);
err_sw_init:
err_adminq_setup:
err_pf_reset:
	iounmap(hw->hw_addr);
err_ioremap:
	kfree(pf);
err_pf_alloc:
	pci_disable_pcie_error_reporting(pdev);
	pci_release_mem_regions(pdev);
err_pci_reg:
err_dma:
	pci_disable_device(pdev);
	return err;
}