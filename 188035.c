static int prb_calc_retire_blk_tmo(struct packet_sock *po,
				int blk_size_in_bytes)
{
	struct net_device *dev;
	unsigned int mbits = 0, msec = 0, div = 0, tmo = 0;

	dev = dev_get_by_index(sock_net(&po->sk), po->ifindex);
	if (unlikely(dev == NULL))
		return DEFAULT_PRB_RETIRE_TOV;

	if (dev->ethtool_ops && dev->ethtool_ops->get_settings) {
		struct ethtool_cmd ecmd = { .cmd = ETHTOOL_GSET, };

		if (!dev->ethtool_ops->get_settings(dev, &ecmd)) {
			switch (ecmd.speed) {
			case SPEED_10000:
				msec = 1;
				div = 10000/1000;
				break;
			case SPEED_1000:
				msec = 1;
				div = 1000/1000;
				break;
			/*
			 * If the link speed is so slow you don't really
			 * need to worry about perf anyways
			 */
			case SPEED_100:
			case SPEED_10:
			default:
				return DEFAULT_PRB_RETIRE_TOV;
			}
		}
	}

	mbits = (blk_size_in_bytes * 8) / (1024 * 1024);

	if (div)
		mbits /= div;

	tmo = mbits * msec;

	if (div)
		return tmo+1;
	return tmo;
}