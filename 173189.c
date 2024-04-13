static void bnx2x_process_kill_chip_reset(struct bnx2x *bp, bool global)
{
	u32 not_reset_mask1, reset_mask1, not_reset_mask2, reset_mask2;
	u32 global_bits2, stay_reset2;

	/*
	 * Bits that have to be set in reset_mask2 if we want to reset 'global'
	 * (per chip) blocks.
	 */
	global_bits2 =
		MISC_REGISTERS_RESET_REG_2_RST_MCP_N_RESET_CMN_CPU |
		MISC_REGISTERS_RESET_REG_2_RST_MCP_N_RESET_CMN_CORE;

	/* Don't reset the following blocks.
	 * Important: per port blocks (such as EMAC, BMAC, UMAC) can't be
	 *            reset, as in 4 port device they might still be owned
	 *            by the MCP (there is only one leader per path).
	 */
	not_reset_mask1 =
		MISC_REGISTERS_RESET_REG_1_RST_HC |
		MISC_REGISTERS_RESET_REG_1_RST_PXPV |
		MISC_REGISTERS_RESET_REG_1_RST_PXP;

	not_reset_mask2 =
		MISC_REGISTERS_RESET_REG_2_RST_PCI_MDIO |
		MISC_REGISTERS_RESET_REG_2_RST_EMAC0_HARD_CORE |
		MISC_REGISTERS_RESET_REG_2_RST_EMAC1_HARD_CORE |
		MISC_REGISTERS_RESET_REG_2_RST_MISC_CORE |
		MISC_REGISTERS_RESET_REG_2_RST_RBCN |
		MISC_REGISTERS_RESET_REG_2_RST_GRC  |
		MISC_REGISTERS_RESET_REG_2_RST_MCP_N_RESET_REG_HARD_CORE |
		MISC_REGISTERS_RESET_REG_2_RST_MCP_N_HARD_CORE_RST_B |
		MISC_REGISTERS_RESET_REG_2_RST_ATC |
		MISC_REGISTERS_RESET_REG_2_PGLC |
		MISC_REGISTERS_RESET_REG_2_RST_BMAC0 |
		MISC_REGISTERS_RESET_REG_2_RST_BMAC1 |
		MISC_REGISTERS_RESET_REG_2_RST_EMAC0 |
		MISC_REGISTERS_RESET_REG_2_RST_EMAC1 |
		MISC_REGISTERS_RESET_REG_2_UMAC0 |
		MISC_REGISTERS_RESET_REG_2_UMAC1;

	/*
	 * Keep the following blocks in reset:
	 *  - all xxMACs are handled by the bnx2x_link code.
	 */
	stay_reset2 =
		MISC_REGISTERS_RESET_REG_2_XMAC |
		MISC_REGISTERS_RESET_REG_2_XMAC_SOFT;

	/* Full reset masks according to the chip */
	reset_mask1 = 0xffffffff;

	if (CHIP_IS_E1(bp))
		reset_mask2 = 0xffff;
	else if (CHIP_IS_E1H(bp))
		reset_mask2 = 0x1ffff;
	else if (CHIP_IS_E2(bp))
		reset_mask2 = 0xfffff;
	else /* CHIP_IS_E3 */
		reset_mask2 = 0x3ffffff;

	/* Don't reset global blocks unless we need to */
	if (!global)
		reset_mask2 &= ~global_bits2;

	/*
	 * In case of attention in the QM, we need to reset PXP
	 * (MISC_REGISTERS_RESET_REG_2_RST_PXP_RQ_RD_WR) before QM
	 * because otherwise QM reset would release 'close the gates' shortly
	 * before resetting the PXP, then the PSWRQ would send a write
	 * request to PGLUE. Then when PXP is reset, PGLUE would try to
	 * read the payload data from PSWWR, but PSWWR would not
	 * respond. The write queue in PGLUE would stuck, dmae commands
	 * would not return. Therefore it's important to reset the second
	 * reset register (containing the
	 * MISC_REGISTERS_RESET_REG_2_RST_PXP_RQ_RD_WR bit) before the
	 * first one (containing the MISC_REGISTERS_RESET_REG_1_RST_QM
	 * bit).
	 */
	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_2_CLEAR,
	       reset_mask2 & (~not_reset_mask2));

	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_1_CLEAR,
	       reset_mask1 & (~not_reset_mask1));

	barrier();
	mmiowb();

	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_2_SET,
	       reset_mask2 & (~stay_reset2));

	barrier();
	mmiowb();

	REG_WR(bp, GRCBASE_MISC + MISC_REGISTERS_RESET_REG_1_SET, reset_mask1);
	mmiowb();
}