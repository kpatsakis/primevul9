static void bnx2x_reset_func(struct bnx2x *bp)
{
	int port = BP_PORT(bp);
	int func = BP_FUNC(bp);
	int i;

	/* Disable the function in the FW */
	REG_WR8(bp, BAR_XSTRORM_INTMEM + XSTORM_FUNC_EN_OFFSET(func), 0);
	REG_WR8(bp, BAR_CSTRORM_INTMEM + CSTORM_FUNC_EN_OFFSET(func), 0);
	REG_WR8(bp, BAR_TSTRORM_INTMEM + TSTORM_FUNC_EN_OFFSET(func), 0);
	REG_WR8(bp, BAR_USTRORM_INTMEM + USTORM_FUNC_EN_OFFSET(func), 0);

	/* FP SBs */
	for_each_eth_queue(bp, i) {
		struct bnx2x_fastpath *fp = &bp->fp[i];
		REG_WR8(bp, BAR_CSTRORM_INTMEM +
			   CSTORM_STATUS_BLOCK_DATA_STATE_OFFSET(fp->fw_sb_id),
			   SB_DISABLED);
	}

	if (CNIC_LOADED(bp))
		/* CNIC SB */
		REG_WR8(bp, BAR_CSTRORM_INTMEM +
			CSTORM_STATUS_BLOCK_DATA_STATE_OFFSET
			(bnx2x_cnic_fw_sb_id(bp)), SB_DISABLED);

	/* SP SB */
	REG_WR8(bp, BAR_CSTRORM_INTMEM +
		CSTORM_SP_STATUS_BLOCK_DATA_STATE_OFFSET(func),
		SB_DISABLED);

	for (i = 0; i < XSTORM_SPQ_DATA_SIZE / 4; i++)
		REG_WR(bp, BAR_XSTRORM_INTMEM + XSTORM_SPQ_DATA_OFFSET(func),
		       0);

	/* Configure IGU */
	if (bp->common.int_block == INT_BLOCK_HC) {
		REG_WR(bp, HC_REG_LEADING_EDGE_0 + port*8, 0);
		REG_WR(bp, HC_REG_TRAILING_EDGE_0 + port*8, 0);
	} else {
		REG_WR(bp, IGU_REG_LEADING_EDGE_LATCH, 0);
		REG_WR(bp, IGU_REG_TRAILING_EDGE_LATCH, 0);
	}

	if (CNIC_LOADED(bp)) {
		/* Disable Timer scan */
		REG_WR(bp, TM_REG_EN_LINEAR0_TIMER + port*4, 0);
		/*
		 * Wait for at least 10ms and up to 2 second for the timers
		 * scan to complete
		 */
		for (i = 0; i < 200; i++) {
			usleep_range(10000, 20000);
			if (!REG_RD(bp, TM_REG_LIN0_SCAN_ON + port*4))
				break;
		}
	}
	/* Clear ILT */
	bnx2x_clear_func_ilt(bp, func);

	/* Timers workaround bug for E2: if this is vnic-3,
	 * we need to set the entire ilt range for this timers.
	 */
	if (!CHIP_IS_E1x(bp) && BP_VN(bp) == 3) {
		struct ilt_client_info ilt_cli;
		/* use dummy TM client */
		memset(&ilt_cli, 0, sizeof(struct ilt_client_info));
		ilt_cli.start = 0;
		ilt_cli.end = ILT_NUM_PAGE_ENTRIES - 1;
		ilt_cli.client_num = ILT_CLIENT_TM;

		bnx2x_ilt_boundry_init_op(bp, &ilt_cli, 0, INITOP_CLEAR);
	}

	/* this assumes that reset_port() called before reset_func()*/
	if (!CHIP_IS_E1x(bp))
		bnx2x_pf_disable(bp);

	bp->dmae_ready = 0;
}