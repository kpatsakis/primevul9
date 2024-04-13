int dccp_init_sock(struct sock *sk, const __u8 ctl_sock_initialized)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct inet_connection_sock *icsk = inet_csk(sk);

	icsk->icsk_rto		= DCCP_TIMEOUT_INIT;
	icsk->icsk_syn_retries	= sysctl_dccp_request_retries;
	sk->sk_state		= DCCP_CLOSED;
	sk->sk_write_space	= dccp_write_space;
	sk->sk_destruct		= dccp_sk_destruct;
	icsk->icsk_sync_mss	= dccp_sync_mss;
	dp->dccps_mss_cache	= 536;
	dp->dccps_rate_last	= jiffies;
	dp->dccps_role		= DCCP_ROLE_UNDEFINED;
	dp->dccps_service	= DCCP_SERVICE_CODE_IS_ABSENT;
	dp->dccps_tx_qlen	= sysctl_dccp_tx_qlen;

	dccp_init_xmit_timers(sk);

	INIT_LIST_HEAD(&dp->dccps_featneg);
	/* control socket doesn't need feat nego */
	if (likely(ctl_sock_initialized))
		return dccp_feat_init(sk);
	return 0;
}