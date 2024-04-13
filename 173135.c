static int bnx2x_int_mem_test(struct bnx2x *bp)
{
	int factor;
	int count, i;
	u32 val = 0;

	if (CHIP_REV_IS_FPGA(bp))
		factor = 120;
	else if (CHIP_REV_IS_EMUL(bp))
		factor = 200;
	else
		factor = 1;

	/* Disable inputs of parser neighbor blocks */
	REG_WR(bp, TSDM_REG_ENABLE_IN1, 0x0);
	REG_WR(bp, TCM_REG_PRS_IFEN, 0x0);
	REG_WR(bp, CFC_REG_DEBUG0, 0x1);
	REG_WR(bp, NIG_REG_PRS_REQ_IN_EN, 0x0);

	/*  Write 0 to parser credits for CFC search request */
	REG_WR(bp, PRS_REG_CFC_SEARCH_INITIAL_CREDIT, 0x0);

	/* send Ethernet packet */
	bnx2x_lb_pckt(bp);

	/* TODO do i reset NIG statistic? */
	/* Wait until NIG register shows 1 packet of size 0x10 */
	count = 1000 * factor;
	while (count) {

		bnx2x_read_dmae(bp, NIG_REG_STAT2_BRB_OCTET, 2);
		val = *bnx2x_sp(bp, wb_data[0]);
		if (val == 0x10)
			break;

		usleep_range(10000, 20000);
		count--;
	}
	if (val != 0x10) {
		BNX2X_ERR("NIG timeout  val = 0x%x\n", val);
		return -1;
	}

	/* Wait until PRS register shows 1 packet */
	count = 1000 * factor;
	while (count) {
		val = REG_RD(bp, PRS_REG_NUM_OF_PACKETS);
		if (val == 1)
			break;

		usleep_range(10000, 20000);
		count--;
	}
	if (val != 0x1) {
		BNX2X_ERR("PRS timeout val = 0x%x\n", val);
		return -2;
	}

	/* Reset and init BRB, PRS */
	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_1_CLEAR, 0x03);
	msleep(50);
	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_1_SET, 0x03);
	msleep(50);
	bnx2x_init_block(bp, BLOCK_BRB1, PHASE_COMMON);
	bnx2x_init_block(bp, BLOCK_PRS, PHASE_COMMON);

	DP(NETIF_MSG_HW, "part2\n");

	/* Disable inputs of parser neighbor blocks */
	REG_WR(bp, TSDM_REG_ENABLE_IN1, 0x0);
	REG_WR(bp, TCM_REG_PRS_IFEN, 0x0);
	REG_WR(bp, CFC_REG_DEBUG0, 0x1);
	REG_WR(bp, NIG_REG_PRS_REQ_IN_EN, 0x0);

	/* Write 0 to parser credits for CFC search request */
	REG_WR(bp, PRS_REG_CFC_SEARCH_INITIAL_CREDIT, 0x0);

	/* send 10 Ethernet packets */
	for (i = 0; i < 10; i++)
		bnx2x_lb_pckt(bp);

	/* Wait until NIG register shows 10 + 1
	   packets of size 11*0x10 = 0xb0 */
	count = 1000 * factor;
	while (count) {

		bnx2x_read_dmae(bp, NIG_REG_STAT2_BRB_OCTET, 2);
		val = *bnx2x_sp(bp, wb_data[0]);
		if (val == 0xb0)
			break;

		usleep_range(10000, 20000);
		count--;
	}
	if (val != 0xb0) {
		BNX2X_ERR("NIG timeout  val = 0x%x\n", val);
		return -3;
	}

	/* Wait until PRS register shows 2 packets */
	val = REG_RD(bp, PRS_REG_NUM_OF_PACKETS);
	if (val != 2)
		BNX2X_ERR("PRS timeout  val = 0x%x\n", val);

	/* Write 1 to parser credits for CFC search request */
	REG_WR(bp, PRS_REG_CFC_SEARCH_INITIAL_CREDIT, 0x1);

	/* Wait until PRS register shows 3 packets */
	msleep(10 * factor);
	/* Wait until NIG register shows 1 packet of size 0x10 */
	val = REG_RD(bp, PRS_REG_NUM_OF_PACKETS);
	if (val != 3)
		BNX2X_ERR("PRS timeout  val = 0x%x\n", val);

	/* clear NIG EOP FIFO */
	for (i = 0; i < 11; i++)
		REG_RD(bp, NIG_REG_INGRESS_EOP_LB_FIFO);
	val = REG_RD(bp, NIG_REG_INGRESS_EOP_LB_EMPTY);
	if (val != 1) {
		BNX2X_ERR("clear of NIG failed\n");
		return -4;
	}

	/* Reset and init BRB, PRS, NIG */
	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_1_CLEAR, 0x03);
	msleep(50);
	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_1_SET, 0x03);
	msleep(50);
	bnx2x_init_block(bp, BLOCK_BRB1, PHASE_COMMON);
	bnx2x_init_block(bp, BLOCK_PRS, PHASE_COMMON);
	if (!CNIC_SUPPORT(bp))
		/* set NIC mode */
		REG_WR(bp, PRS_REG_NIC_MODE, 1);

	/* Enable inputs of parser neighbor blocks */
	REG_WR(bp, TSDM_REG_ENABLE_IN1, 0x7fffffff);
	REG_WR(bp, TCM_REG_PRS_IFEN, 0x1);
	REG_WR(bp, CFC_REG_DEBUG0, 0x0);
	REG_WR(bp, NIG_REG_PRS_REQ_IN_EN, 0x1);

	DP(NETIF_MSG_HW, "done\n");

	return 0; /* OK */
}