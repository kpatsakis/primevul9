static int bnx2x_set_mc_list(struct bnx2x *bp)
{
	LIST_HEAD(mcast_group_list);
	struct bnx2x_mcast_ramrod_params rparam = {NULL};
	struct net_device *dev = bp->dev;
	int rc = 0;

	/* On older adapters, we need to flush and re-add filters */
	if (CHIP_IS_E1x(bp))
		return bnx2x_set_mc_list_e1x(bp);

	rparam.mcast_obj = &bp->mcast_obj;

	if (netdev_mc_count(dev)) {
		rc = bnx2x_init_mcast_macs_list(bp, &rparam, &mcast_group_list);
		if (rc)
			return rc;

		/* Override the curently configured set of mc filters */
		rc = bnx2x_config_mcast(bp, &rparam,
					BNX2X_MCAST_CMD_SET);
		if (rc < 0)
			BNX2X_ERR("Failed to set a new multicast configuration: %d\n",
				  rc);

		bnx2x_free_mcast_macs_list(&mcast_group_list);
	} else {
		/* If no mc addresses are required, flush the configuration */
		rc = bnx2x_config_mcast(bp, &rparam, BNX2X_MCAST_CMD_DEL);
		if (rc < 0)
			BNX2X_ERR("Failed to clear multicast configuration %d\n",
				  rc);
	}

	return rc;
}