static void bnx2x_timer(struct timer_list *t)
{
	struct bnx2x *bp = from_timer(bp, t, timer);

	if (!netif_running(bp->dev))
		return;

	if (IS_PF(bp) &&
	    !BP_NOMCP(bp)) {
		int mb_idx = BP_FW_MB_IDX(bp);
		u16 drv_pulse;
		u16 mcp_pulse;

		++bp->fw_drv_pulse_wr_seq;
		bp->fw_drv_pulse_wr_seq &= DRV_PULSE_SEQ_MASK;
		drv_pulse = bp->fw_drv_pulse_wr_seq;
		bnx2x_drv_pulse(bp);

		mcp_pulse = (SHMEM_RD(bp, func_mb[mb_idx].mcp_pulse_mb) &
			     MCP_PULSE_SEQ_MASK);
		/* The delta between driver pulse and mcp response
		 * should not get too big. If the MFW is more than 5 pulses
		 * behind, we should worry about it enough to generate an error
		 * log.
		 */
		if (((drv_pulse - mcp_pulse) & MCP_PULSE_SEQ_MASK) > 5)
			BNX2X_ERR("MFW seems hanged: drv_pulse (0x%x) != mcp_pulse (0x%x)\n",
				  drv_pulse, mcp_pulse);
	}

	if (bp->state == BNX2X_STATE_OPEN)
		bnx2x_stats_handle(bp, STATS_EVENT_UPDATE);

	/* sample pf vf bulletin board for new posts from pf */
	if (IS_VF(bp))
		bnx2x_timer_sriov(bp);

	mod_timer(&bp->timer, jiffies + bp->current_interval);
}