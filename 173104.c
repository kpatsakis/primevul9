static int bnx2x_pf_flr_clnup(struct bnx2x *bp)
{
	u32 poll_cnt = bnx2x_flr_clnup_poll_count(bp);

	DP(BNX2X_MSG_SP, "Cleanup after FLR PF[%d]\n", BP_ABS_FUNC(bp));

	/* Re-enable PF target read access */
	REG_WR(bp, PGLUE_B_REG_INTERNAL_PFID_ENABLE_TARGET_READ, 1);

	/* Poll HW usage counters */
	DP(BNX2X_MSG_SP, "Polling usage counters\n");
	if (bnx2x_poll_hw_usage_counters(bp, poll_cnt))
		return -EBUSY;

	/* Zero the igu 'trailing edge' and 'leading edge' */

	/* Send the FW cleanup command */
	if (bnx2x_send_final_clnup(bp, (u8)BP_FUNC(bp), poll_cnt))
		return -EBUSY;

	/* ATC cleanup */

	/* Verify TX hw is flushed */
	bnx2x_tx_hw_flushed(bp, poll_cnt);

	/* Wait 100ms (not adjusted according to platform) */
	msleep(100);

	/* Verify no pending pci transactions */
	if (bnx2x_is_pcie_pending(bp->pdev))
		BNX2X_ERR("PCIE Transactions still pending\n");

	/* Debug */
	bnx2x_hw_enable_status(bp);

	/*
	 * Master enable - Due to WB DMAE writes performed before this
	 * register is re-initialized as part of the regular function init
	 */
	REG_WR(bp, PGLUE_B_REG_INTERNAL_PFID_ENABLE_MASTER, 1);

	return 0;
}