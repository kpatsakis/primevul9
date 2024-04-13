static int i40e_sw_init(struct i40e_pf *pf)
{
	int err = 0;
	int size;

	/* Set default capability flags */
	pf->flags = I40E_FLAG_RX_CSUM_ENABLED |
		    I40E_FLAG_MSI_ENABLED     |
		    I40E_FLAG_MSIX_ENABLED;

	/* Set default ITR */
	pf->rx_itr_default = I40E_ITR_RX_DEF;
	pf->tx_itr_default = I40E_ITR_TX_DEF;

	/* Depending on PF configurations, it is possible that the RSS
	 * maximum might end up larger than the available queues
	 */
	pf->rss_size_max = BIT(pf->hw.func_caps.rss_table_entry_width);
	pf->alloc_rss_size = 1;
	pf->rss_table_size = pf->hw.func_caps.rss_table_size;
	pf->rss_size_max = min_t(int, pf->rss_size_max,
				 pf->hw.func_caps.num_tx_qp);
	if (pf->hw.func_caps.rss) {
		pf->flags |= I40E_FLAG_RSS_ENABLED;
		pf->alloc_rss_size = min_t(int, pf->rss_size_max,
					   num_online_cpus());
	}

	/* MFP mode enabled */
	if (pf->hw.func_caps.npar_enable || pf->hw.func_caps.flex10_enable) {
		pf->flags |= I40E_FLAG_MFP_ENABLED;
		dev_info(&pf->pdev->dev, "MFP mode Enabled\n");
		if (i40e_get_partition_bw_setting(pf)) {
			dev_warn(&pf->pdev->dev,
				 "Could not get partition bw settings\n");
		} else {
			dev_info(&pf->pdev->dev,
				 "Partition BW Min = %8.8x, Max = %8.8x\n",
				 pf->min_bw, pf->max_bw);

			/* nudge the Tx scheduler */
			i40e_set_partition_bw_setting(pf);
		}
	}

	if ((pf->hw.func_caps.fd_filters_guaranteed > 0) ||
	    (pf->hw.func_caps.fd_filters_best_effort > 0)) {
		pf->flags |= I40E_FLAG_FD_ATR_ENABLED;
		pf->atr_sample_rate = I40E_DEFAULT_ATR_SAMPLE_RATE;
		if (pf->flags & I40E_FLAG_MFP_ENABLED &&
		    pf->hw.num_partitions > 1)
			dev_info(&pf->pdev->dev,
				 "Flow Director Sideband mode Disabled in MFP mode\n");
		else
			pf->flags |= I40E_FLAG_FD_SB_ENABLED;
		pf->fdir_pf_filter_count =
				 pf->hw.func_caps.fd_filters_guaranteed;
		pf->hw.fdir_shared_filter_count =
				 pf->hw.func_caps.fd_filters_best_effort;
	}

	if (pf->hw.mac.type == I40E_MAC_X722) {
		pf->hw_features |= (I40E_HW_RSS_AQ_CAPABLE |
				    I40E_HW_128_QP_RSS_CAPABLE |
				    I40E_HW_ATR_EVICT_CAPABLE |
				    I40E_HW_WB_ON_ITR_CAPABLE |
				    I40E_HW_MULTIPLE_TCP_UDP_RSS_PCTYPE |
				    I40E_HW_NO_PCI_LINK_CHECK |
				    I40E_HW_USE_SET_LLDP_MIB |
				    I40E_HW_GENEVE_OFFLOAD_CAPABLE |
				    I40E_HW_PTP_L4_CAPABLE |
				    I40E_HW_WOL_MC_MAGIC_PKT_WAKE |
				    I40E_HW_OUTER_UDP_CSUM_CAPABLE);

#define I40E_FDEVICT_PCTYPE_DEFAULT 0xc03
		if (rd32(&pf->hw, I40E_GLQF_FDEVICTENA(1)) !=
		    I40E_FDEVICT_PCTYPE_DEFAULT) {
			dev_warn(&pf->pdev->dev,
				 "FD EVICT PCTYPES are not right, disable FD HW EVICT\n");
			pf->hw_features &= ~I40E_HW_ATR_EVICT_CAPABLE;
		}
	} else if ((pf->hw.aq.api_maj_ver > 1) ||
		   ((pf->hw.aq.api_maj_ver == 1) &&
		    (pf->hw.aq.api_min_ver > 4))) {
		/* Supported in FW API version higher than 1.4 */
		pf->hw_features |= I40E_HW_GENEVE_OFFLOAD_CAPABLE;
	}

	/* Enable HW ATR eviction if possible */
	if (pf->hw_features & I40E_HW_ATR_EVICT_CAPABLE)
		pf->flags |= I40E_FLAG_HW_ATR_EVICT_ENABLED;

	if ((pf->hw.mac.type == I40E_MAC_XL710) &&
	    (((pf->hw.aq.fw_maj_ver == 4) && (pf->hw.aq.fw_min_ver < 33)) ||
	    (pf->hw.aq.fw_maj_ver < 4))) {
		pf->hw_features |= I40E_HW_RESTART_AUTONEG;
		/* No DCB support  for FW < v4.33 */
		pf->hw_features |= I40E_HW_NO_DCB_SUPPORT;
	}

	/* Disable FW LLDP if FW < v4.3 */
	if ((pf->hw.mac.type == I40E_MAC_XL710) &&
	    (((pf->hw.aq.fw_maj_ver == 4) && (pf->hw.aq.fw_min_ver < 3)) ||
	    (pf->hw.aq.fw_maj_ver < 4)))
		pf->hw_features |= I40E_HW_STOP_FW_LLDP;

	/* Use the FW Set LLDP MIB API if FW > v4.40 */
	if ((pf->hw.mac.type == I40E_MAC_XL710) &&
	    (((pf->hw.aq.fw_maj_ver == 4) && (pf->hw.aq.fw_min_ver >= 40)) ||
	    (pf->hw.aq.fw_maj_ver >= 5)))
		pf->hw_features |= I40E_HW_USE_SET_LLDP_MIB;

	/* Enable PTP L4 if FW > v6.0 */
	if (pf->hw.mac.type == I40E_MAC_XL710 &&
	    pf->hw.aq.fw_maj_ver >= 6)
		pf->hw_features |= I40E_HW_PTP_L4_CAPABLE;

	if (pf->hw.func_caps.vmdq && num_online_cpus() != 1) {
		pf->num_vmdq_vsis = I40E_DEFAULT_NUM_VMDQ_VSI;
		pf->flags |= I40E_FLAG_VMDQ_ENABLED;
		pf->num_vmdq_qps = i40e_default_queues_per_vmdq(pf);
	}

	if (pf->hw.func_caps.iwarp && num_online_cpus() != 1) {
		pf->flags |= I40E_FLAG_IWARP_ENABLED;
		/* IWARP needs one extra vector for CQP just like MISC.*/
		pf->num_iwarp_msix = (int)num_online_cpus() + 1;
	}
	/* Stopping FW LLDP engine is supported on XL710 and X722
	 * starting from FW versions determined in i40e_init_adminq.
	 * Stopping the FW LLDP engine is not supported on XL710
	 * if NPAR is functioning so unset this hw flag in this case.
	 */
	if (pf->hw.mac.type == I40E_MAC_XL710 &&
	    pf->hw.func_caps.npar_enable &&
	    (pf->hw.flags & I40E_HW_FLAG_FW_LLDP_STOPPABLE))
		pf->hw.flags &= ~I40E_HW_FLAG_FW_LLDP_STOPPABLE;

#ifdef CONFIG_PCI_IOV
	if (pf->hw.func_caps.num_vfs && pf->hw.partition_id == 1) {
		pf->num_vf_qps = I40E_DEFAULT_QUEUES_PER_VF;
		pf->flags |= I40E_FLAG_SRIOV_ENABLED;
		pf->num_req_vfs = min_t(int,
					pf->hw.func_caps.num_vfs,
					I40E_MAX_VF_COUNT);
	}
#endif /* CONFIG_PCI_IOV */
	pf->eeprom_version = 0xDEAD;
	pf->lan_veb = I40E_NO_VEB;
	pf->lan_vsi = I40E_NO_VSI;

	/* By default FW has this off for performance reasons */
	pf->flags &= ~I40E_FLAG_VEB_STATS_ENABLED;

	/* set up queue assignment tracking */
	size = sizeof(struct i40e_lump_tracking)
		+ (sizeof(u16) * pf->hw.func_caps.num_tx_qp);
	pf->qp_pile = kzalloc(size, GFP_KERNEL);
	if (!pf->qp_pile) {
		err = -ENOMEM;
		goto sw_init_done;
	}
	pf->qp_pile->num_entries = pf->hw.func_caps.num_tx_qp;
	pf->qp_pile->search_hint = 0;

	pf->tx_timeout_recovery_level = 1;

	mutex_init(&pf->switch_mutex);

sw_init_done:
	return err;
}