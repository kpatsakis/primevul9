static void tcp_probe_timer(struct sock *sk)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	struct sk_buff *skb = tcp_send_head(sk);
	struct tcp_sock *tp = tcp_sk(sk);
	int max_probes;

	if (tp->packets_out || !skb) {
		icsk->icsk_probes_out = 0;
		return;
	}

	/* RFC 1122 4.2.2.17 requires the sender to stay open indefinitely as
	 * long as the receiver continues to respond probes. We support this by
	 * default and reset icsk_probes_out with incoming ACKs. But if the
	 * socket is orphaned or the user specifies TCP_USER_TIMEOUT, we
	 * kill the socket when the retry count and the time exceeds the
	 * corresponding system limit. We also implement similar policy when
	 * we use RTO to probe window in tcp_retransmit_timer().
	 */
	if (icsk->icsk_user_timeout) {
		u32 elapsed = tcp_model_timeout(sk, icsk->icsk_probes_out,
						tcp_probe0_base(sk));

		if (elapsed >= icsk->icsk_user_timeout)
			goto abort;
	}

	max_probes = sock_net(sk)->ipv4.sysctl_tcp_retries2;
	if (sock_flag(sk, SOCK_DEAD)) {
		const bool alive = inet_csk_rto_backoff(icsk, TCP_RTO_MAX) < TCP_RTO_MAX;

		max_probes = tcp_orphan_retries(sk, alive);
		if (!alive && icsk->icsk_backoff >= max_probes)
			goto abort;
		if (tcp_out_of_resources(sk, true))
			return;
	}

	if (icsk->icsk_probes_out >= max_probes) {
abort:		tcp_write_err(sk);
	} else {
		/* Only send another probe if we didn't close things up. */
		tcp_send_probe0(sk);
	}
}