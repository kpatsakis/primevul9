void bnx2x_set_rx_ts(struct bnx2x *bp, struct sk_buff *skb)
{
	int port = BP_PORT(bp);
	u64 timestamp, ns;

	timestamp = REG_RD(bp, port ? NIG_REG_P1_LLH_PTP_HOST_BUF_TS_MSB :
			    NIG_REG_P0_LLH_PTP_HOST_BUF_TS_MSB);
	timestamp <<= 32;
	timestamp |= REG_RD(bp, port ? NIG_REG_P1_LLH_PTP_HOST_BUF_TS_LSB :
			    NIG_REG_P0_LLH_PTP_HOST_BUF_TS_LSB);

	/* Reset timestamp register to allow new timestamp */
	REG_WR(bp, port ? NIG_REG_P1_LLH_PTP_HOST_BUF_SEQID :
	       NIG_REG_P0_LLH_PTP_HOST_BUF_SEQID, 0x10000);

	ns = timecounter_cyc2time(&bp->timecounter, timestamp);

	skb_hwtstamps(skb)->hwtstamp = ns_to_ktime(ns);

	DP(BNX2X_MSG_PTP, "Rx timestamp, timestamp cycles = %llu, ns = %llu\n",
	   timestamp, ns);
}