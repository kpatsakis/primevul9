static int bnx2x_process_kill(struct bnx2x *bp, bool global)
{
	int cnt = 1000;
	u32 val = 0;
	u32 sr_cnt, blk_cnt, port_is_idle_0, port_is_idle_1, pgl_exp_rom2;
	u32 tags_63_32 = 0;

	/* Empty the Tetris buffer, wait for 1s */
	do {
		sr_cnt  = REG_RD(bp, PXP2_REG_RD_SR_CNT);
		blk_cnt = REG_RD(bp, PXP2_REG_RD_BLK_CNT);
		port_is_idle_0 = REG_RD(bp, PXP2_REG_RD_PORT_IS_IDLE_0);
		port_is_idle_1 = REG_RD(bp, PXP2_REG_RD_PORT_IS_IDLE_1);
		pgl_exp_rom2 = REG_RD(bp, PXP2_REG_PGL_EXP_ROM2);
		if (CHIP_IS_E3(bp))
			tags_63_32 = REG_RD(bp, PGLUE_B_REG_TAGS_63_32);

		if ((sr_cnt == 0x7e) && (blk_cnt == 0xa0) &&
		    ((port_is_idle_0 & 0x1) == 0x1) &&
		    ((port_is_idle_1 & 0x1) == 0x1) &&
		    (pgl_exp_rom2 == 0xffffffff) &&
		    (!CHIP_IS_E3(bp) || (tags_63_32 == 0xffffffff)))
			break;
		usleep_range(1000, 2000);
	} while (cnt-- > 0);

	if (cnt <= 0) {
		BNX2X_ERR("Tetris buffer didn't get empty or there are still outstanding read requests after 1s!\n");
		BNX2X_ERR("sr_cnt=0x%08x, blk_cnt=0x%08x, port_is_idle_0=0x%08x, port_is_idle_1=0x%08x, pgl_exp_rom2=0x%08x\n",
			  sr_cnt, blk_cnt, port_is_idle_0, port_is_idle_1,
			  pgl_exp_rom2);
		return -EAGAIN;
	}

	barrier();

	/* Close gates #2, #3 and #4 */
	bnx2x_set_234_gates(bp, true);

	/* Poll for IGU VQs for 57712 and newer chips */
	if (!CHIP_IS_E1x(bp) && bnx2x_er_poll_igu_vq(bp))
		return -EAGAIN;

	/* TBD: Indicate that "process kill" is in progress to MCP */

	/* Clear "unprepared" bit */
	REG_WR(bp, MISC_REG_UNPREPARED, 0);
	barrier();

	/* Make sure all is written to the chip before the reset */
	mmiowb();

	/* Wait for 1ms to empty GLUE and PCI-E core queues,
	 * PSWHST, GRC and PSWRD Tetris buffer.
	 */
	usleep_range(1000, 2000);

	/* Prepare to chip reset: */
	/* MCP */
	if (global)
		bnx2x_reset_mcp_prep(bp, &val);

	/* PXP */
	bnx2x_pxp_prep(bp);
	barrier();

	/* reset the chip */
	bnx2x_process_kill_chip_reset(bp, global);
	barrier();

	/* clear errors in PGB */
	if (!CHIP_IS_E1x(bp))
		REG_WR(bp, PGLUE_B_REG_LATCHED_ERRORS_CLR, 0x7f);

	/* Recover after reset: */
	/* MCP */
	if (global && bnx2x_reset_mcp_comp(bp, val))
		return -EAGAIN;

	/* TBD: Add resetting the NO_MCP mode DB here */

	/* Open the gates #2, #3 and #4 */
	bnx2x_set_234_gates(bp, false);

	/* TBD: IGU/AEU preparation bring back the AEU/IGU to a
	 * reset state, re-enable attentions. */

	return 0;
}