static void bnx2x_handle_rx_mode_eqe(struct bnx2x *bp)
{
	netif_addr_lock_bh(bp->dev);

	clear_bit(BNX2X_FILTER_RX_MODE_PENDING, &bp->sp_state);

	/* Send rx_mode command again if was requested */
	if (test_and_clear_bit(BNX2X_FILTER_RX_MODE_SCHED, &bp->sp_state))
		bnx2x_set_storm_rx_mode(bp);
	else if (test_and_clear_bit(BNX2X_FILTER_ISCSI_ETH_START_SCHED,
				    &bp->sp_state))
		bnx2x_set_iscsi_eth_rx_mode(bp, true);
	else if (test_and_clear_bit(BNX2X_FILTER_ISCSI_ETH_STOP_SCHED,
				    &bp->sp_state))
		bnx2x_set_iscsi_eth_rx_mode(bp, false);

	netif_addr_unlock_bh(bp->dev);
}