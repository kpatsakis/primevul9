static void bnx2x_stop_ptp(struct bnx2x *bp)
{
	/* Cancel PTP work queue. Should be done after the Tx queues are
	 * drained to prevent additional scheduling.
	 */
	cancel_work_sync(&bp->ptp_task);

	if (bp->ptp_tx_skb) {
		dev_kfree_skb_any(bp->ptp_tx_skb);
		bp->ptp_tx_skb = NULL;
	}

	/* Disable PTP in HW */
	bnx2x_disable_ptp(bp);

	DP(BNX2X_MSG_PTP, "PTP stop ended successfully\n");
}