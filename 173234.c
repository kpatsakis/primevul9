static void bnx2x_e1h_enable(struct bnx2x *bp)
{
	int port = BP_PORT(bp);

	if (!(IS_MF_UFP(bp) && BNX2X_IS_MF_SD_PROTOCOL_FCOE(bp)))
		REG_WR(bp, NIG_REG_LLH0_FUNC_EN + port * 8, 1);

	/* Tx queue should be only re-enabled */
	netif_tx_wake_all_queues(bp->dev);

	/*
	 * Should not call netif_carrier_on since it will be called if the link
	 * is up when checking for link state
	 */
}