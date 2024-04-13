static void bnx2x_pmf_update(struct bnx2x *bp)
{
	int port = BP_PORT(bp);
	u32 val;

	bp->port.pmf = 1;
	DP(BNX2X_MSG_MCP, "pmf %d\n", bp->port.pmf);

	/*
	 * We need the mb() to ensure the ordering between the writing to
	 * bp->port.pmf here and reading it from the bnx2x_periodic_task().
	 */
	smp_mb();

	/* queue a periodic task */
	queue_delayed_work(bnx2x_wq, &bp->period_task, 0);

	bnx2x_dcbx_pmf_update(bp);

	/* enable nig attention */
	val = (0xff0f | (1 << (BP_VN(bp) + 4)));
	if (bp->common.int_block == INT_BLOCK_HC) {
		REG_WR(bp, HC_REG_TRAILING_EDGE_0 + port*8, val);
		REG_WR(bp, HC_REG_LEADING_EDGE_0 + port*8, val);
	} else if (!CHIP_IS_E1x(bp)) {
		REG_WR(bp, IGU_REG_TRAILING_EDGE_LATCH, val);
		REG_WR(bp, IGU_REG_LEADING_EDGE_LATCH, val);
	}

	bnx2x_stats_handle(bp, STATS_EVENT_PMF);
}