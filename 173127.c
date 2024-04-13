static int bnx2x_set_mc_list_e1x(struct bnx2x *bp)
{
	LIST_HEAD(mcast_group_list);
	struct net_device *dev = bp->dev;
	struct bnx2x_mcast_ramrod_params rparam = {NULL};
	int rc = 0;

	rparam.mcast_obj = &bp->mcast_obj;

	/* first, clear all configured multicast MACs */
	rc = bnx2x_config_mcast(bp, &rparam, BNX2X_MCAST_CMD_DEL);
	if (rc < 0) {
		BNX2X_ERR("Failed to clear multicast configuration: %d\n", rc);
		return rc;
	}

	/* then, configure a new MACs list */
	if (netdev_mc_count(dev)) {
		rc = bnx2x_init_mcast_macs_list(bp, &rparam, &mcast_group_list);
		if (rc)
			return rc;

		/* Now add the new MACs */
		rc = bnx2x_config_mcast(bp, &rparam,
					BNX2X_MCAST_CMD_ADD);
		if (rc < 0)
			BNX2X_ERR("Failed to set a new multicast configuration: %d\n",
				  rc);

		bnx2x_free_mcast_macs_list(&mcast_group_list);
	}

	return rc;
}