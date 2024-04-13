static int bnx2x_ptp_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
	struct bnx2x *bp = container_of(ptp, struct bnx2x, ptp_clock_info);
	u64 ns;

	if (!netif_running(bp->dev)) {
		DP(BNX2X_MSG_PTP,
		   "PTP gettime called while the interface is down\n");
		return -ENETDOWN;
	}

	ns = timecounter_read(&bp->timecounter);

	DP(BNX2X_MSG_PTP, "PTP gettime called, ns = %llu\n", ns);

	*ts = ns_to_timespec64(ns);

	return 0;
}