void tcp_write_timer_handler(struct sock *sk)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	int event;

	if (((1 << sk->sk_state) & (TCPF_CLOSE | TCPF_LISTEN)) ||
	    !icsk->icsk_pending)
		goto out;

	if (time_after(icsk->icsk_timeout, jiffies)) {
		sk_reset_timer(sk, &icsk->icsk_retransmit_timer, icsk->icsk_timeout);
		goto out;
	}

	tcp_mstamp_refresh(tcp_sk(sk));
	event = icsk->icsk_pending;

	switch (event) {
	case ICSK_TIME_REO_TIMEOUT:
		tcp_rack_reo_timeout(sk);
		break;
	case ICSK_TIME_LOSS_PROBE:
		tcp_send_loss_probe(sk);
		break;
	case ICSK_TIME_RETRANS:
		icsk->icsk_pending = 0;
		tcp_retransmit_timer(sk);
		break;
	case ICSK_TIME_PROBE0:
		icsk->icsk_pending = 0;
		tcp_probe_timer(sk);
		break;
	}

out:
	sk_mem_reclaim(sk);
}