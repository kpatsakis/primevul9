static int bnx2x_func_wait_started(struct bnx2x *bp)
{
	int tout = 50;
	int msix = (bp->flags & USING_MSIX_FLAG) ? 1 : 0;

	if (!bp->port.pmf)
		return 0;

	/*
	 * (assumption: No Attention from MCP at this stage)
	 * PMF probably in the middle of TX disable/enable transaction
	 * 1. Sync IRS for default SB
	 * 2. Sync SP queue - this guarantees us that attention handling started
	 * 3. Wait, that TX disable/enable transaction completes
	 *
	 * 1+2 guarantee that if DCBx attention was scheduled it already changed
	 * pending bit of transaction from STARTED-->TX_STOPPED, if we already
	 * received completion for the transaction the state is TX_STOPPED.
	 * State will return to STARTED after completion of TX_STOPPED-->STARTED
	 * transaction.
	 */

	/* make sure default SB ISR is done */
	if (msix)
		synchronize_irq(bp->msix_table[0].vector);
	else
		synchronize_irq(bp->pdev->irq);

	flush_workqueue(bnx2x_wq);
	flush_workqueue(bnx2x_iov_wq);

	while (bnx2x_func_get_state(bp, &bp->func_obj) !=
				BNX2X_F_STATE_STARTED && tout--)
		msleep(20);

	if (bnx2x_func_get_state(bp, &bp->func_obj) !=
						BNX2X_F_STATE_STARTED) {
#ifdef BNX2X_STOP_ON_ERROR
		BNX2X_ERR("Wrong function state\n");
		return -EBUSY;
#else
		/*
		 * Failed to complete the transaction in a "good way"
		 * Force both transactions with CLR bit
		 */
		struct bnx2x_func_state_params func_params = {NULL};

		DP(NETIF_MSG_IFDOWN,
		   "Hmmm... Unexpected function state! Forcing STARTED-->TX_STOPPED-->STARTED\n");

		func_params.f_obj = &bp->func_obj;
		__set_bit(RAMROD_DRV_CLR_ONLY,
					&func_params.ramrod_flags);

		/* STARTED-->TX_ST0PPED */
		func_params.cmd = BNX2X_F_CMD_TX_STOP;
		bnx2x_func_state_change(bp, &func_params);

		/* TX_ST0PPED-->STARTED */
		func_params.cmd = BNX2X_F_CMD_TX_START;
		return bnx2x_func_state_change(bp, &func_params);
#endif
	}

	return 0;
}