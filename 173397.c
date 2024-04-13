static int bnx2x_ptp_adjfreq(struct ptp_clock_info *ptp, s32 ppb)
{
	struct bnx2x *bp = container_of(ptp, struct bnx2x, ptp_clock_info);
	int rc;
	int drift_dir = 1;
	int val, period, period1, period2, dif, dif1, dif2;
	int best_dif = BNX2X_MAX_PHC_DRIFT, best_period = 0, best_val = 0;

	DP(BNX2X_MSG_PTP, "PTP adjfreq called, ppb = %d\n", ppb);

	if (!netif_running(bp->dev)) {
		DP(BNX2X_MSG_PTP,
		   "PTP adjfreq called while the interface is down\n");
		return -ENETDOWN;
	}

	if (ppb < 0) {
		ppb = -ppb;
		drift_dir = 0;
	}

	if (ppb == 0) {
		best_val = 1;
		best_period = 0x1FFFFFF;
	} else if (ppb >= BNX2X_MAX_PHC_DRIFT) {
		best_val = 31;
		best_period = 1;
	} else {
		/* Changed not to allow val = 8, 16, 24 as these values
		 * are not supported in workaround.
		 */
		for (val = 0; val <= 31; val++) {
			if ((val & 0x7) == 0)
				continue;
			period1 = val * 1000000 / ppb;
			period2 = period1 + 1;
			if (period1 != 0)
				dif1 = ppb - (val * 1000000 / period1);
			else
				dif1 = BNX2X_MAX_PHC_DRIFT;
			if (dif1 < 0)
				dif1 = -dif1;
			dif2 = ppb - (val * 1000000 / period2);
			if (dif2 < 0)
				dif2 = -dif2;
			dif = (dif1 < dif2) ? dif1 : dif2;
			period = (dif1 < dif2) ? period1 : period2;
			if (dif < best_dif) {
				best_dif = dif;
				best_val = val;
				best_period = period;
			}
		}
	}

	rc = bnx2x_send_update_drift_ramrod(bp, drift_dir, best_val,
					    best_period);
	if (rc) {
		BNX2X_ERR("Failed to set drift\n");
		return -EFAULT;
	}

	DP(BNX2X_MSG_PTP, "Configured val = %d, period = %d\n", best_val,
	   best_period);

	return 0;
}