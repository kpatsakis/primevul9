static void bnx2x_set_234_gates(struct bnx2x *bp, bool close)
{
	u32 val;

	/* Gates #2 and #4a are closed/opened for "not E1" only */
	if (!CHIP_IS_E1(bp)) {
		/* #4 */
		REG_WR(bp, PXP_REG_HST_DISCARD_DOORBELLS, !!close);
		/* #2 */
		REG_WR(bp, PXP_REG_HST_DISCARD_INTERNAL_WRITES, !!close);
	}

	/* #3 */
	if (CHIP_IS_E1x(bp)) {
		/* Prevent interrupts from HC on both ports */
		val = REG_RD(bp, HC_REG_CONFIG_1);
		REG_WR(bp, HC_REG_CONFIG_1,
		       (!close) ? (val | HC_CONFIG_1_REG_BLOCK_DISABLE_1) :
		       (val & ~(u32)HC_CONFIG_1_REG_BLOCK_DISABLE_1));

		val = REG_RD(bp, HC_REG_CONFIG_0);
		REG_WR(bp, HC_REG_CONFIG_0,
		       (!close) ? (val | HC_CONFIG_0_REG_BLOCK_DISABLE_0) :
		       (val & ~(u32)HC_CONFIG_0_REG_BLOCK_DISABLE_0));
	} else {
		/* Prevent incoming interrupts in IGU */
		val = REG_RD(bp, IGU_REG_BLOCK_CONFIGURATION);

		REG_WR(bp, IGU_REG_BLOCK_CONFIGURATION,
		       (!close) ?
		       (val | IGU_BLOCK_CONFIGURATION_REG_BLOCK_ENABLE) :
		       (val & ~(u32)IGU_BLOCK_CONFIGURATION_REG_BLOCK_ENABLE));
	}

	DP(NETIF_MSG_HW | NETIF_MSG_IFUP, "%s gates #2, #3 and #4\n",
		close ? "closing" : "opening");
	mmiowb();
}