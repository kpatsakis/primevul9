unsigned int dccp_sync_mss(struct sock *sk, u32 pmtu)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	struct dccp_sock *dp = dccp_sk(sk);
	int mss_now = (pmtu - icsk->icsk_af_ops->net_header_len -
		       sizeof(struct dccp_hdr) - sizeof(struct dccp_hdr_ext));

	/* Now subtract optional transport overhead */
	mss_now -= icsk->icsk_ext_hdr_len;

	/*
	 * FIXME: this should come from the CCID infrastructure, where, say,
	 * TFRC will say it wants TIMESTAMPS, ELAPSED time, etc, for now lets
	 * put a rough estimate for NDP + TIMESTAMP + TIMESTAMP_ECHO + ELAPSED
	 * TIME + TFRC_OPT_LOSS_EVENT_RATE + TFRC_OPT_RECEIVE_RATE + padding to
	 * make it a multiple of 4
	 */

	mss_now -= ((5 + 6 + 10 + 6 + 6 + 6 + 3) / 4) * 4;

	/* And store cached results */
	icsk->icsk_pmtu_cookie = pmtu;
	dp->dccps_mss_cache = mss_now;

	return mss_now;
}