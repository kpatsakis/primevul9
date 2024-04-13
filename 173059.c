static int bnx2x_ptp_settime(struct ptp_clock_info *ptp,
			     const struct timespec64 *ts)
{
	struct bnx2x *bp = container_of(ptp, struct bnx2x, ptp_clock_info);
	u64 ns;

	if (!netif_running(bp->dev)) {
		DP(BNX2X_MSG_PTP,
		   "PTP settime called while the interface is down\n");
		return -ENETDOWN;
	}

	ns = timespec64_to_ns(ts);

	DP(BNX2X_MSG_PTP, "PTP settime called, ns = %llu\n", ns);

	/* Re-init the timecounter */
	timecounter_init(&bp->timecounter, &bp->cyclecounter, ns);

	return 0;
}