static inline void dccp_connect_init(struct sock *sk)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct dst_entry *dst = __sk_dst_get(sk);
	struct inet_connection_sock *icsk = inet_csk(sk);

	sk->sk_err = 0;
	sock_reset_flag(sk, SOCK_DONE);

	dccp_sync_mss(sk, dst_mtu(dst));

	/*
	 * SWL and AWL are initially adjusted so that they are not less than
	 * the initial Sequence Numbers received and sent, respectively:
	 *	SWL := max(GSR + 1 - floor(W/4), ISR),
	 *	AWL := max(GSS - W' + 1, ISS).
	 * These adjustments MUST be applied only at the beginning of the
	 * connection.
	 */
	dccp_update_gss(sk, dp->dccps_iss);
	dccp_set_seqno(&dp->dccps_awl, max48(dp->dccps_awl, dp->dccps_iss));

	/* S.GAR - greatest valid acknowledgement number received on a non-Sync;
	 *         initialized to S.ISS (sec. 8.5)                            */
	dp->dccps_gar = dp->dccps_iss;

	icsk->icsk_retransmits = 0;
}