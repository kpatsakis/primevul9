static void bnx2x_set_rx_mode(struct net_device *dev)
{
	struct bnx2x *bp = netdev_priv(dev);

	if (bp->state != BNX2X_STATE_OPEN) {
		DP(NETIF_MSG_IFUP, "state is %x, returning\n", bp->state);
		return;
	} else {
		/* Schedule an SP task to handle rest of change */
		bnx2x_schedule_sp_rtnl(bp, BNX2X_SP_RTNL_RX_MODE,
				       NETIF_MSG_IFUP);
	}
}