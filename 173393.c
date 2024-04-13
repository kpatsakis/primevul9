static void bnx2x_handle_mcast_eqe(struct bnx2x *bp)
{
	struct bnx2x_mcast_ramrod_params rparam;
	int rc;

	memset(&rparam, 0, sizeof(rparam));

	rparam.mcast_obj = &bp->mcast_obj;

	netif_addr_lock_bh(bp->dev);

	/* Clear pending state for the last command */
	bp->mcast_obj.raw.clear_pending(&bp->mcast_obj.raw);

	/* If there are pending mcast commands - send them */
	if (bp->mcast_obj.check_pending(&bp->mcast_obj)) {
		rc = bnx2x_config_mcast(bp, &rparam, BNX2X_MCAST_CMD_CONT);
		if (rc < 0)
			BNX2X_ERR("Failed to send pending mcast commands: %d\n",
				  rc);
	}

	netif_addr_unlock_bh(bp->dev);
}