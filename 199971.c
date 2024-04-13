int dccp_init_sock(struct sock *sk, const __u8 ctl_sock_initialized)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct dccp_minisock *dmsk = dccp_msk(sk);
	struct inet_connection_sock *icsk = inet_csk(sk);

	dccp_minisock_init(&dp->dccps_minisock);
	do_gettimeofday(&dp->dccps_epoch);

	/*
	 * FIXME: We're hardcoding the CCID, and doing this at this point makes
	 * the listening (master) sock get CCID control blocks, which is not
	 * necessary, but for now, to not mess with the test userspace apps,
	 * lets leave it here, later the real solution is to do this in a
	 * setsockopt(CCIDs-I-want/accept). -acme
	 */
	if (likely(ctl_sock_initialized)) {
		int rc = dccp_feat_init(dmsk);

		if (rc)
			return rc;

		if (dmsk->dccpms_send_ack_vector) {
			dp->dccps_hc_rx_ackvec = dccp_ackvec_alloc(GFP_KERNEL);
			if (dp->dccps_hc_rx_ackvec == NULL)
				return -ENOMEM;
		}
		dp->dccps_hc_rx_ccid = ccid_hc_rx_new(dmsk->dccpms_rx_ccid,
						      sk, GFP_KERNEL);
		dp->dccps_hc_tx_ccid = ccid_hc_tx_new(dmsk->dccpms_tx_ccid,
						      sk, GFP_KERNEL);
		if (unlikely(dp->dccps_hc_rx_ccid == NULL ||
			     dp->dccps_hc_tx_ccid == NULL)) {
			ccid_hc_rx_delete(dp->dccps_hc_rx_ccid, sk);
			ccid_hc_tx_delete(dp->dccps_hc_tx_ccid, sk);
			if (dmsk->dccpms_send_ack_vector) {
				dccp_ackvec_free(dp->dccps_hc_rx_ackvec);
				dp->dccps_hc_rx_ackvec = NULL;
			}
			dp->dccps_hc_rx_ccid = dp->dccps_hc_tx_ccid = NULL;
			return -ENOMEM;
		}
	} else {
		/* control socket doesn't need feat nego */
		INIT_LIST_HEAD(&dmsk->dccpms_pending);
		INIT_LIST_HEAD(&dmsk->dccpms_conf);
	}

	dccp_init_xmit_timers(sk);
	icsk->icsk_rto		= DCCP_TIMEOUT_INIT;
	icsk->icsk_syn_retries	= sysctl_dccp_request_retries;
	sk->sk_state		= DCCP_CLOSED;
	sk->sk_write_space	= dccp_write_space;
	icsk->icsk_sync_mss	= dccp_sync_mss;
	dp->dccps_mss_cache	= 536;
	dp->dccps_role		= DCCP_ROLE_UNDEFINED;
	dp->dccps_service	= DCCP_SERVICE_CODE_IS_ABSENT;
	dp->dccps_l_ack_ratio	= dp->dccps_r_ack_ratio = 1;

	return 0;
}