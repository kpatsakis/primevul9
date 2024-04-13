static void bnx2x_get_mac_hwinfo(struct bnx2x *bp)
{
	u32 val, val2;
	int func = BP_ABS_FUNC(bp);
	int port = BP_PORT(bp);

	/* Zero primary MAC configuration */
	eth_zero_addr(bp->dev->dev_addr);

	if (BP_NOMCP(bp)) {
		BNX2X_ERROR("warning: random MAC workaround active\n");
		eth_hw_addr_random(bp->dev);
	} else if (IS_MF(bp)) {
		val2 = MF_CFG_RD(bp, func_mf_config[func].mac_upper);
		val = MF_CFG_RD(bp, func_mf_config[func].mac_lower);
		if ((val2 != FUNC_MF_CFG_UPPERMAC_DEFAULT) &&
		    (val != FUNC_MF_CFG_LOWERMAC_DEFAULT))
			bnx2x_set_mac_buf(bp->dev->dev_addr, val, val2);

		if (CNIC_SUPPORT(bp))
			bnx2x_get_cnic_mac_hwinfo(bp);
	} else {
		/* in SF read MACs from port configuration */
		val2 = SHMEM_RD(bp, dev_info.port_hw_config[port].mac_upper);
		val = SHMEM_RD(bp, dev_info.port_hw_config[port].mac_lower);
		bnx2x_set_mac_buf(bp->dev->dev_addr, val, val2);

		if (CNIC_SUPPORT(bp))
			bnx2x_get_cnic_mac_hwinfo(bp);
	}

	if (!BP_NOMCP(bp)) {
		/* Read physical port identifier from shmem */
		val2 = SHMEM_RD(bp, dev_info.port_hw_config[port].mac_upper);
		val = SHMEM_RD(bp, dev_info.port_hw_config[port].mac_lower);
		bnx2x_set_mac_buf(bp->phys_port_id, val, val2);
		bp->flags |= HAS_PHYS_PORT_ID;
	}

	memcpy(bp->link_params.mac_addr, bp->dev->dev_addr, ETH_ALEN);

	if (!is_valid_ether_addr(bp->dev->dev_addr))
		dev_err(&bp->pdev->dev,
			"bad Ethernet MAC address configuration: %pM\n"
			"change it manually before bringing up the appropriate network interface\n",
			bp->dev->dev_addr);
}