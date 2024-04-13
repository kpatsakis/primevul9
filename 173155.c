static int bnx2x_ptp_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
	struct bnx2x *bp = container_of(ptp, struct bnx2x, ptp_clock_info);

	if (!netif_running(bp->dev)) {
		DP(BNX2X_MSG_PTP,
		   "PTP adjtime called while the interface is down\n");
		return -ENETDOWN;
	}

	DP(BNX2X_MSG_PTP, "PTP adjtime called, delta = %llx\n", delta);

	timecounter_adjtime(&bp->timecounter, delta);

	return 0;
}