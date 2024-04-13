static int bnx2x_hwtstamp_ioctl(struct bnx2x *bp, struct ifreq *ifr)
{
	struct hwtstamp_config config;
	int rc;

	DP(BNX2X_MSG_PTP, "HWTSTAMP IOCTL called\n");

	if (copy_from_user(&config, ifr->ifr_data, sizeof(config)))
		return -EFAULT;

	DP(BNX2X_MSG_PTP, "Requested tx_type: %d, requested rx_filters = %d\n",
	   config.tx_type, config.rx_filter);

	if (config.flags) {
		BNX2X_ERR("config.flags is reserved for future use\n");
		return -EINVAL;
	}

	bp->hwtstamp_ioctl_called = 1;
	bp->tx_type = config.tx_type;
	bp->rx_filter = config.rx_filter;

	rc = bnx2x_configure_ptp_filters(bp);
	if (rc)
		return rc;

	config.rx_filter = bp->rx_filter;

	return copy_to_user(ifr->ifr_data, &config, sizeof(config)) ?
		-EFAULT : 0;
}