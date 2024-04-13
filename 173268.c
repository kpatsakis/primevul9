static int bnx2x_init_one(struct pci_dev *pdev,
				    const struct pci_device_id *ent)
{
	struct net_device *dev = NULL;
	struct bnx2x *bp;
	enum pcie_link_width pcie_width;
	enum pci_bus_speed pcie_speed;
	int rc, max_non_def_sbs;
	int rx_count, tx_count, rss_count, doorbell_size;
	int max_cos_est;
	bool is_vf;
	int cnic_cnt;

	/* Management FW 'remembers' living interfaces. Allow it some time
	 * to forget previously living interfaces, allowing a proper re-load.
	 */
	if (is_kdump_kernel()) {
		ktime_t now = ktime_get_boottime();
		ktime_t fw_ready_time = ktime_set(5, 0);

		if (ktime_before(now, fw_ready_time))
			msleep(ktime_ms_delta(fw_ready_time, now));
	}

	/* An estimated maximum supported CoS number according to the chip
	 * version.
	 * We will try to roughly estimate the maximum number of CoSes this chip
	 * may support in order to minimize the memory allocated for Tx
	 * netdev_queue's. This number will be accurately calculated during the
	 * initialization of bp->max_cos based on the chip versions AND chip
	 * revision in the bnx2x_init_bp().
	 */
	max_cos_est = set_max_cos_est(ent->driver_data);
	if (max_cos_est < 0)
		return max_cos_est;
	is_vf = set_is_vf(ent->driver_data);
	cnic_cnt = is_vf ? 0 : 1;

	max_non_def_sbs = bnx2x_get_num_non_def_sbs(pdev, cnic_cnt);

	/* add another SB for VF as it has no default SB */
	max_non_def_sbs += is_vf ? 1 : 0;

	/* Maximum number of RSS queues: one IGU SB goes to CNIC */
	rss_count = max_non_def_sbs - cnic_cnt;

	if (rss_count < 1)
		return -EINVAL;

	/* Maximum number of netdev Rx queues: RSS + FCoE L2 */
	rx_count = rss_count + cnic_cnt;

	/* Maximum number of netdev Tx queues:
	 * Maximum TSS queues * Maximum supported number of CoS  + FCoE L2
	 */
	tx_count = rss_count * max_cos_est + cnic_cnt;

	/* dev zeroed in init_etherdev */
	dev = alloc_etherdev_mqs(sizeof(*bp), tx_count, rx_count);
	if (!dev)
		return -ENOMEM;

	bp = netdev_priv(dev);

	bp->flags = 0;
	if (is_vf)
		bp->flags |= IS_VF_FLAG;

	bp->igu_sb_cnt = max_non_def_sbs;
	bp->igu_base_addr = IS_VF(bp) ? PXP_VF_ADDR_IGU_START : BAR_IGU_INTMEM;
	bp->msg_enable = debug;
	bp->cnic_support = cnic_cnt;
	bp->cnic_probe = bnx2x_cnic_probe;

	pci_set_drvdata(pdev, dev);

	rc = bnx2x_init_dev(bp, pdev, dev, ent->driver_data);
	if (rc < 0) {
		free_netdev(dev);
		return rc;
	}

	BNX2X_DEV_INFO("This is a %s function\n",
		       IS_PF(bp) ? "physical" : "virtual");
	BNX2X_DEV_INFO("Cnic support is %s\n", CNIC_SUPPORT(bp) ? "on" : "off");
	BNX2X_DEV_INFO("Max num of status blocks %d\n", max_non_def_sbs);
	BNX2X_DEV_INFO("Allocated netdev with %d tx and %d rx queues\n",
		       tx_count, rx_count);

	rc = bnx2x_init_bp(bp);
	if (rc)
		goto init_one_exit;

	/* Map doorbells here as we need the real value of bp->max_cos which
	 * is initialized in bnx2x_init_bp() to determine the number of
	 * l2 connections.
	 */
	if (IS_VF(bp)) {
		bp->doorbells = bnx2x_vf_doorbells(bp);
		rc = bnx2x_vf_pci_alloc(bp);
		if (rc)
			goto init_one_freemem;
	} else {
		doorbell_size = BNX2X_L2_MAX_CID(bp) * (1 << BNX2X_DB_SHIFT);
		if (doorbell_size > pci_resource_len(pdev, 2)) {
			dev_err(&bp->pdev->dev,
				"Cannot map doorbells, bar size too small, aborting\n");
			rc = -ENOMEM;
			goto init_one_freemem;
		}
		bp->doorbells = ioremap_nocache(pci_resource_start(pdev, 2),
						doorbell_size);
	}
	if (!bp->doorbells) {
		dev_err(&bp->pdev->dev,
			"Cannot map doorbell space, aborting\n");
		rc = -ENOMEM;
		goto init_one_freemem;
	}

	if (IS_VF(bp)) {
		rc = bnx2x_vfpf_acquire(bp, tx_count, rx_count);
		if (rc)
			goto init_one_freemem;

#ifdef CONFIG_BNX2X_SRIOV
		/* VF with OLD Hypervisor or old PF do not support filtering */
		if (bp->acquire_resp.pfdev_info.pf_cap & PFVF_CAP_VLAN_FILTER) {
			dev->hw_features |= NETIF_F_HW_VLAN_CTAG_FILTER;
			dev->features |= NETIF_F_HW_VLAN_CTAG_FILTER;
		}
#endif
	}

	/* Enable SRIOV if capability found in configuration space */
	rc = bnx2x_iov_init_one(bp, int_mode, BNX2X_MAX_NUM_OF_VFS);
	if (rc)
		goto init_one_freemem;

	/* calc qm_cid_count */
	bp->qm_cid_count = bnx2x_set_qm_cid_count(bp);
	BNX2X_DEV_INFO("qm_cid_count %d\n", bp->qm_cid_count);

	/* disable FCOE L2 queue for E1x*/
	if (CHIP_IS_E1x(bp))
		bp->flags |= NO_FCOE_FLAG;

	/* Set bp->num_queues for MSI-X mode*/
	bnx2x_set_num_queues(bp);

	/* Configure interrupt mode: try to enable MSI-X/MSI if
	 * needed.
	 */
	rc = bnx2x_set_int_mode(bp);
	if (rc) {
		dev_err(&pdev->dev, "Cannot set interrupts\n");
		goto init_one_freemem;
	}
	BNX2X_DEV_INFO("set interrupts successfully\n");

	/* register the net device */
	rc = register_netdev(dev);
	if (rc) {
		dev_err(&pdev->dev, "Cannot register net device\n");
		goto init_one_freemem;
	}
	BNX2X_DEV_INFO("device name after netdev register %s\n", dev->name);

	if (!NO_FCOE(bp)) {
		/* Add storage MAC address */
		rtnl_lock();
		dev_addr_add(bp->dev, bp->fip_mac, NETDEV_HW_ADDR_T_SAN);
		rtnl_unlock();
	}
	if (pcie_get_minimum_link(bp->pdev, &pcie_speed, &pcie_width) ||
	    pcie_speed == PCI_SPEED_UNKNOWN ||
	    pcie_width == PCIE_LNK_WIDTH_UNKNOWN)
		BNX2X_DEV_INFO("Failed to determine PCI Express Bandwidth\n");
	else
		BNX2X_DEV_INFO(
		       "%s (%c%d) PCI-E x%d %s found at mem %lx, IRQ %d, node addr %pM\n",
		       board_info[ent->driver_data].name,
		       (CHIP_REV(bp) >> 12) + 'A', (CHIP_METAL(bp) >> 4),
		       pcie_width,
		       pcie_speed == PCIE_SPEED_2_5GT ? "2.5GHz" :
		       pcie_speed == PCIE_SPEED_5_0GT ? "5.0GHz" :
		       pcie_speed == PCIE_SPEED_8_0GT ? "8.0GHz" :
		       "Unknown",
		       dev->base_addr, bp->pdev->irq, dev->dev_addr);

	bnx2x_register_phc(bp);

	if (!IS_MF_SD_STORAGE_PERSONALITY_ONLY(bp))
		bnx2x_set_os_driver_state(bp, OS_DRIVER_STATE_DISABLED);

	return 0;

init_one_freemem:
	bnx2x_free_mem_bp(bp);

init_one_exit:
	bnx2x_disable_pcie_error_reporting(bp);

	if (bp->regview)
		iounmap(bp->regview);

	if (IS_PF(bp) && bp->doorbells)
		iounmap(bp->doorbells);

	free_netdev(dev);

	if (atomic_read(&pdev->enable_cnt) == 1)
		pci_release_regions(pdev);

	pci_disable_device(pdev);

	return rc;
}