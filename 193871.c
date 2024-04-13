void tcp_delack_timer_handler(struct sock *sk)
{
	struct inet_connection_sock *icsk = inet_csk(sk);

	sk_mem_reclaim_partial(sk);

	if (((1 << sk->sk_state) & (TCPF_CLOSE | TCPF_LISTEN)) ||
	    !(icsk->icsk_ack.pending & ICSK_ACK_TIMER))
		goto out;

	if (time_after(icsk->icsk_ack.timeout, jiffies)) {
		sk_reset_timer(sk, &icsk->icsk_delack_timer, icsk->icsk_ack.timeout);
		goto out;
	}
	icsk->icsk_ack.pending &= ~ICSK_ACK_TIMER;

	if (inet_csk_ack_scheduled(sk)) {
		if (!inet_csk_in_pingpong_mode(sk)) {
			/* Delayed ACK missed: inflate ATO. */
			icsk->icsk_ack.ato = min(icsk->icsk_ack.ato << 1, icsk->icsk_rto);
		} else {
			/* Delayed ACK missed: leave pingpong mode and
			 * deflate ATO.
			 */
			inet_csk_exit_pingpong_mode(sk);
			icsk->icsk_ack.ato      = TCP_ATO_MIN;
		}
		tcp_mstamp_refresh(tcp_sk(sk));
		tcp_send_ack(sk);
		__NET_INC_STATS(sock_net(sk), LINUX_MIB_DELAYEDACKS);
	}

out:
	if (tcp_under_memory_pressure(sk))
		sk_mem_reclaim(sk);
}