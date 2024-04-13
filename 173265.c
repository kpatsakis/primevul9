static int bnx2x_reset_nic_mode(struct bnx2x *bp)
{
	int rc, i, port = BP_PORT(bp);
	int vlan_en = 0, mac_en[NUM_MACS];

	/* Close input from network */
	if (bp->mf_mode == SINGLE_FUNCTION) {
		bnx2x_set_rx_filter(&bp->link_params, 0);
	} else {
		vlan_en = REG_RD(bp, port ? NIG_REG_LLH1_FUNC_EN :
				   NIG_REG_LLH0_FUNC_EN);
		REG_WR(bp, port ? NIG_REG_LLH1_FUNC_EN :
			  NIG_REG_LLH0_FUNC_EN, 0);
		for (i = 0; i < NUM_MACS; i++) {
			mac_en[i] = REG_RD(bp, port ?
					     (NIG_REG_LLH1_FUNC_MEM_ENABLE +
					      4 * i) :
					     (NIG_REG_LLH0_FUNC_MEM_ENABLE +
					      4 * i));
			REG_WR(bp, port ? (NIG_REG_LLH1_FUNC_MEM_ENABLE +
					      4 * i) :
				  (NIG_REG_LLH0_FUNC_MEM_ENABLE + 4 * i), 0);
		}
	}

	/* Close BMC to host */
	REG_WR(bp, port ? NIG_REG_P0_TX_MNG_HOST_ENABLE :
	       NIG_REG_P1_TX_MNG_HOST_ENABLE, 0);

	/* Suspend Tx switching to the PF. Completion of this ramrod
	 * further guarantees that all the packets of that PF / child
	 * VFs in BRB were processed by the Parser, so it is safe to
	 * change the NIC_MODE register.
	 */
	rc = bnx2x_func_switch_update(bp, 1);
	if (rc) {
		BNX2X_ERR("Can't suspend tx-switching!\n");
		return rc;
	}

	/* Change NIC_MODE register */
	REG_WR(bp, PRS_REG_NIC_MODE, 0);

	/* Open input from network */
	if (bp->mf_mode == SINGLE_FUNCTION) {
		bnx2x_set_rx_filter(&bp->link_params, 1);
	} else {
		REG_WR(bp, port ? NIG_REG_LLH1_FUNC_EN :
			  NIG_REG_LLH0_FUNC_EN, vlan_en);
		for (i = 0; i < NUM_MACS; i++) {
			REG_WR(bp, port ? (NIG_REG_LLH1_FUNC_MEM_ENABLE +
					      4 * i) :
				  (NIG_REG_LLH0_FUNC_MEM_ENABLE + 4 * i),
				  mac_en[i]);
		}
	}

	/* Enable BMC to host */
	REG_WR(bp, port ? NIG_REG_P0_TX_MNG_HOST_ENABLE :
	       NIG_REG_P1_TX_MNG_HOST_ENABLE, 1);

	/* Resume Tx switching to the PF */
	rc = bnx2x_func_switch_update(bp, 0);
	if (rc) {
		BNX2X_ERR("Can't resume tx-switching!\n");
		return rc;
	}

	DP(NETIF_MSG_IFUP, "NIC MODE disabled\n");
	return 0;
}