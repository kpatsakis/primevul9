static int tcp_write_timeout(struct sock *sk)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	struct tcp_sock *tp = tcp_sk(sk);
	struct net *net = sock_net(sk);
	bool expired, do_reset;
	int retry_until;

	if ((1 << sk->sk_state) & (TCPF_SYN_SENT | TCPF_SYN_RECV)) {
		if (icsk->icsk_retransmits) {
			dst_negative_advice(sk);
		} else {
			sk_rethink_txhash(sk);
		}
		retry_until = icsk->icsk_syn_retries ? : net->ipv4.sysctl_tcp_syn_retries;
		expired = icsk->icsk_retransmits >= retry_until;
	} else {
		if (retransmits_timed_out(sk, net->ipv4.sysctl_tcp_retries1, 0)) {
			/* Black hole detection */
			tcp_mtu_probing(icsk, sk);

			dst_negative_advice(sk);
		} else {
			sk_rethink_txhash(sk);
		}

		retry_until = net->ipv4.sysctl_tcp_retries2;
		if (sock_flag(sk, SOCK_DEAD)) {
			const bool alive = icsk->icsk_rto < TCP_RTO_MAX;

			retry_until = tcp_orphan_retries(sk, alive);
			do_reset = alive ||
				!retransmits_timed_out(sk, retry_until, 0);

			if (tcp_out_of_resources(sk, do_reset))
				return 1;
		}
		expired = retransmits_timed_out(sk, retry_until,
						icsk->icsk_user_timeout);
	}
	tcp_fastopen_active_detect_blackhole(sk, expired);

	if (BPF_SOCK_OPS_TEST_FLAG(tp, BPF_SOCK_OPS_RTO_CB_FLAG))
		tcp_call_bpf_3arg(sk, BPF_SOCK_OPS_RTO_CB,
				  icsk->icsk_retransmits,
				  icsk->icsk_rto, (int)expired);

	if (expired) {
		/* Has it gone just too far? */
		tcp_write_err(sk);
		return 1;
	}

	return 0;
}