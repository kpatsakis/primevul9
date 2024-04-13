static void bnx2x_ptp_task(struct work_struct *work)
{
	struct bnx2x *bp = container_of(work, struct bnx2x, ptp_task);
	int port = BP_PORT(bp);
	u32 val_seq;
	u64 timestamp, ns;
	struct skb_shared_hwtstamps shhwtstamps;

	/* Read Tx timestamp registers */
	val_seq = REG_RD(bp, port ? NIG_REG_P1_TLLH_PTP_BUF_SEQID :
			 NIG_REG_P0_TLLH_PTP_BUF_SEQID);
	if (val_seq & 0x10000) {
		/* There is a valid timestamp value */
		timestamp = REG_RD(bp, port ? NIG_REG_P1_TLLH_PTP_BUF_TS_MSB :
				   NIG_REG_P0_TLLH_PTP_BUF_TS_MSB);
		timestamp <<= 32;
		timestamp |= REG_RD(bp, port ? NIG_REG_P1_TLLH_PTP_BUF_TS_LSB :
				    NIG_REG_P0_TLLH_PTP_BUF_TS_LSB);
		/* Reset timestamp register to allow new timestamp */
		REG_WR(bp, port ? NIG_REG_P1_TLLH_PTP_BUF_SEQID :
		       NIG_REG_P0_TLLH_PTP_BUF_SEQID, 0x10000);
		ns = timecounter_cyc2time(&bp->timecounter, timestamp);

		memset(&shhwtstamps, 0, sizeof(shhwtstamps));
		shhwtstamps.hwtstamp = ns_to_ktime(ns);
		skb_tstamp_tx(bp->ptp_tx_skb, &shhwtstamps);
		dev_kfree_skb_any(bp->ptp_tx_skb);
		bp->ptp_tx_skb = NULL;

		DP(BNX2X_MSG_PTP, "Tx timestamp, timestamp cycles = %llu, ns = %llu\n",
		   timestamp, ns);
	} else {
		DP(BNX2X_MSG_PTP, "There is no valid Tx timestamp yet\n");
		/* Reschedule to keep checking for a valid timestamp value */
		schedule_work(&bp->ptp_task);
	}
}