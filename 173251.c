static int bnx2x_eeh_nic_unload(struct bnx2x *bp)
{
	bp->state = BNX2X_STATE_CLOSING_WAIT4_HALT;

	bp->rx_mode = BNX2X_RX_MODE_NONE;

	if (CNIC_LOADED(bp))
		bnx2x_cnic_notify(bp, CNIC_CTL_STOP_CMD);

	/* Stop Tx */
	bnx2x_tx_disable(bp);
	/* Delete all NAPI objects */
	bnx2x_del_all_napi(bp);
	if (CNIC_LOADED(bp))
		bnx2x_del_all_napi_cnic(bp);
	netdev_reset_tc(bp->dev);

	del_timer_sync(&bp->timer);
	cancel_delayed_work_sync(&bp->sp_task);
	cancel_delayed_work_sync(&bp->period_task);

	if (!down_timeout(&bp->stats_lock, HZ / 10)) {
		bp->stats_state = STATS_STATE_DISABLED;
		up(&bp->stats_lock);
	}

	bnx2x_save_statistics(bp);

	netif_carrier_off(bp->dev);

	return 0;
}