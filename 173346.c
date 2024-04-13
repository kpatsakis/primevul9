static void bnx2x_register_phc(struct bnx2x *bp)
{
	/* Fill the ptp_clock_info struct and register PTP clock*/
	bp->ptp_clock_info.owner = THIS_MODULE;
	snprintf(bp->ptp_clock_info.name, 16, "%s", bp->dev->name);
	bp->ptp_clock_info.max_adj = BNX2X_MAX_PHC_DRIFT; /* In PPB */
	bp->ptp_clock_info.n_alarm = 0;
	bp->ptp_clock_info.n_ext_ts = 0;
	bp->ptp_clock_info.n_per_out = 0;
	bp->ptp_clock_info.pps = 0;
	bp->ptp_clock_info.adjfreq = bnx2x_ptp_adjfreq;
	bp->ptp_clock_info.adjtime = bnx2x_ptp_adjtime;
	bp->ptp_clock_info.gettime64 = bnx2x_ptp_gettime;
	bp->ptp_clock_info.settime64 = bnx2x_ptp_settime;
	bp->ptp_clock_info.enable = bnx2x_ptp_enable;

	bp->ptp_clock = ptp_clock_register(&bp->ptp_clock_info, &bp->pdev->dev);
	if (IS_ERR(bp->ptp_clock)) {
		bp->ptp_clock = NULL;
		BNX2X_ERR("PTP clock registeration failed\n");
	}
}