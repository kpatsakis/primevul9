void bnx2x_init_ptp(struct bnx2x *bp)
{
	int rc;

	/* Configure PTP in HW */
	rc = bnx2x_configure_ptp(bp);
	if (rc) {
		BNX2X_ERR("Stopping PTP initialization\n");
		return;
	}

	/* Init work queue for Tx timestamping */
	INIT_WORK(&bp->ptp_task, bnx2x_ptp_task);

	/* Init cyclecounter and timecounter. This is done only in the first
	 * load. If done in every load, PTP application will fail when doing
	 * unload / load (e.g. MTU change) while it is running.
	 */
	if (!bp->timecounter_init_done) {
		bnx2x_init_cyclecounter(bp);
		timecounter_init(&bp->timecounter, &bp->cyclecounter,
				 ktime_to_ns(ktime_get_real()));
		bp->timecounter_init_done = 1;
	}

	DP(BNX2X_MSG_PTP, "PTP initialization ended successfully\n");
}