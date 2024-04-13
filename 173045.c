static int bnx2x_init_dev(struct bnx2x *bp, struct pci_dev *pdev,
			  struct net_device *dev, unsigned long board_type)
{
	int rc;
	u32 pci_cfg_dword;
	bool chip_is_e1x = (board_type == BCM57710 ||
			    board_type == BCM57711 ||
			    board_type == BCM57711E);

	SET_NETDEV_DEV(dev, &pdev->dev);

	bp->dev = dev;
	bp->pdev = pdev;

	rc = pci_enable_device(pdev);
	if (rc) {
		dev_err(&bp->pdev->dev,
			"Cannot enable PCI device, aborting\n");
		goto err_out;
	}

	if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
		dev_err(&bp->pdev->dev,
			"Cannot find PCI device base address, aborting\n");
		rc = -ENODEV;
		goto err_out_disable;
	}

	if (IS_PF(bp) && !(pci_resource_flags(pdev, 2) & IORESOURCE_MEM)) {
		dev_err(&bp->pdev->dev, "Cannot find second PCI device base address, aborting\n");
		rc = -ENODEV;
		goto err_out_disable;
	}

	pci_read_config_dword(pdev, PCICFG_REVISION_ID_OFFSET, &pci_cfg_dword);
	if ((pci_cfg_dword & PCICFG_REVESION_ID_MASK) ==
	    PCICFG_REVESION_ID_ERROR_VAL) {
		pr_err("PCI device error, probably due to fan failure, aborting\n");
		rc = -ENODEV;
		goto err_out_disable;
	}

	if (atomic_read(&pdev->enable_cnt) == 1) {
		rc = pci_request_regions(pdev, DRV_MODULE_NAME);
		if (rc) {
			dev_err(&bp->pdev->dev,
				"Cannot obtain PCI resources, aborting\n");
			goto err_out_disable;
		}

		pci_set_master(pdev);
		pci_save_state(pdev);
	}

	if (IS_PF(bp)) {
		if (!pdev->pm_cap) {
			dev_err(&bp->pdev->dev,
				"Cannot find power management capability, aborting\n");
			rc = -EIO;
			goto err_out_release;
		}
	}

	if (!pci_is_pcie(pdev)) {
		dev_err(&bp->pdev->dev, "Not PCI Express, aborting\n");
		rc = -EIO;
		goto err_out_release;
	}

	rc = bnx2x_set_coherency_mask(bp);
	if (rc)
		goto err_out_release;

	dev->mem_start = pci_resource_start(pdev, 0);
	dev->base_addr = dev->mem_start;
	dev->mem_end = pci_resource_end(pdev, 0);

	dev->irq = pdev->irq;

	bp->regview = pci_ioremap_bar(pdev, 0);
	if (!bp->regview) {
		dev_err(&bp->pdev->dev,
			"Cannot map register space, aborting\n");
		rc = -ENOMEM;
		goto err_out_release;
	}

	/* In E1/E1H use pci device function given by kernel.
	 * In E2/E3 read physical function from ME register since these chips
	 * support Physical Device Assignment where kernel BDF maybe arbitrary
	 * (depending on hypervisor).
	 */
	if (chip_is_e1x) {
		bp->pf_num = PCI_FUNC(pdev->devfn);
	} else {
		/* chip is E2/3*/
		pci_read_config_dword(bp->pdev,
				      PCICFG_ME_REGISTER, &pci_cfg_dword);
		bp->pf_num = (u8)((pci_cfg_dword & ME_REG_ABS_PF_NUM) >>
				  ME_REG_ABS_PF_NUM_SHIFT);
	}
	BNX2X_DEV_INFO("me reg PF num: %d\n", bp->pf_num);

	/* clean indirect addresses */
	pci_write_config_dword(bp->pdev, PCICFG_GRC_ADDRESS,
			       PCICFG_VENDOR_ID_OFFSET);

	/* Set PCIe reset type to fundamental for EEH recovery */
	pdev->needs_freset = 1;

	/* AER (Advanced Error reporting) configuration */
	rc = pci_enable_pcie_error_reporting(pdev);
	if (!rc)
		bp->flags |= AER_ENABLED;
	else
		BNX2X_DEV_INFO("Failed To configure PCIe AER [%d]\n", rc);

	/*
	 * Clean the following indirect addresses for all functions since it
	 * is not used by the driver.
	 */
	if (IS_PF(bp)) {
		REG_WR(bp, PXP2_REG_PGL_ADDR_88_F0, 0);
		REG_WR(bp, PXP2_REG_PGL_ADDR_8C_F0, 0);
		REG_WR(bp, PXP2_REG_PGL_ADDR_90_F0, 0);
		REG_WR(bp, PXP2_REG_PGL_ADDR_94_F0, 0);

		if (chip_is_e1x) {
			REG_WR(bp, PXP2_REG_PGL_ADDR_88_F1, 0);
			REG_WR(bp, PXP2_REG_PGL_ADDR_8C_F1, 0);
			REG_WR(bp, PXP2_REG_PGL_ADDR_90_F1, 0);
			REG_WR(bp, PXP2_REG_PGL_ADDR_94_F1, 0);
		}

		/* Enable internal target-read (in case we are probed after PF
		 * FLR). Must be done prior to any BAR read access. Only for
		 * 57712 and up
		 */
		if (!chip_is_e1x)
			REG_WR(bp,
			       PGLUE_B_REG_INTERNAL_PFID_ENABLE_TARGET_READ, 1);
	}

	dev->watchdog_timeo = TX_TIMEOUT;

	dev->netdev_ops = &bnx2x_netdev_ops;
	bnx2x_set_ethtool_ops(bp, dev);

	dev->priv_flags |= IFF_UNICAST_FLT;

	dev->hw_features = NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
		NETIF_F_TSO | NETIF_F_TSO_ECN | NETIF_F_TSO6 |
		NETIF_F_RXCSUM | NETIF_F_LRO | NETIF_F_GRO | NETIF_F_GRO_HW |
		NETIF_F_RXHASH | NETIF_F_HW_VLAN_CTAG_TX;
	if (!chip_is_e1x) {
		dev->hw_features |= NETIF_F_GSO_GRE | NETIF_F_GSO_GRE_CSUM |
				    NETIF_F_GSO_IPXIP4 |
				    NETIF_F_GSO_UDP_TUNNEL |
				    NETIF_F_GSO_UDP_TUNNEL_CSUM |
				    NETIF_F_GSO_PARTIAL;

		dev->hw_enc_features =
			NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | NETIF_F_SG |
			NETIF_F_TSO | NETIF_F_TSO_ECN | NETIF_F_TSO6 |
			NETIF_F_GSO_IPXIP4 |
			NETIF_F_GSO_GRE | NETIF_F_GSO_GRE_CSUM |
			NETIF_F_GSO_UDP_TUNNEL | NETIF_F_GSO_UDP_TUNNEL_CSUM |
			NETIF_F_GSO_PARTIAL;

		dev->gso_partial_features = NETIF_F_GSO_GRE_CSUM |
					    NETIF_F_GSO_UDP_TUNNEL_CSUM;
	}

	dev->vlan_features = NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
		NETIF_F_TSO | NETIF_F_TSO_ECN | NETIF_F_TSO6 | NETIF_F_HIGHDMA;

	if (IS_PF(bp)) {
		if (chip_is_e1x)
			bp->accept_any_vlan = true;
		else
			dev->hw_features |= NETIF_F_HW_VLAN_CTAG_FILTER;
	}
	/* For VF we'll know whether to enable VLAN filtering after
	 * getting a response to CHANNEL_TLV_ACQUIRE from PF.
	 */

	dev->features |= dev->hw_features | NETIF_F_HW_VLAN_CTAG_RX;
	dev->features |= NETIF_F_HIGHDMA;
	if (dev->features & NETIF_F_LRO)
		dev->features &= ~NETIF_F_GRO_HW;

	/* Add Loopback capability to the device */
	dev->hw_features |= NETIF_F_LOOPBACK;

#ifdef BCM_DCBNL
	dev->dcbnl_ops = &bnx2x_dcbnl_ops;
#endif

	/* MTU range, 46 - 9600 */
	dev->min_mtu = ETH_MIN_PACKET_SIZE;
	dev->max_mtu = ETH_MAX_JUMBO_PACKET_SIZE;

	/* get_port_hwinfo() will set prtad and mmds properly */
	bp->mdio.prtad = MDIO_PRTAD_NONE;
	bp->mdio.mmds = 0;
	bp->mdio.mode_support = MDIO_SUPPORTS_C45 | MDIO_EMULATE_C22;
	bp->mdio.dev = dev;
	bp->mdio.mdio_read = bnx2x_mdio_read;
	bp->mdio.mdio_write = bnx2x_mdio_write;

	return 0;

err_out_release:
	if (atomic_read(&pdev->enable_cnt) == 1)
		pci_release_regions(pdev);

err_out_disable:
	pci_disable_device(pdev);

err_out:
	return rc;
}