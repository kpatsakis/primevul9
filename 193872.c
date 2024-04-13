void tcp_init_xmit_timers(struct sock *sk)
{
	inet_csk_init_xmit_timers(sk, &tcp_write_timer, &tcp_delack_timer,
				  &tcp_keepalive_timer);
	hrtimer_init(&tcp_sk(sk)->pacing_timer, CLOCK_MONOTONIC,
		     HRTIMER_MODE_ABS_PINNED_SOFT);
	tcp_sk(sk)->pacing_timer.function = tcp_pace_kick;

	hrtimer_init(&tcp_sk(sk)->compressed_ack_timer, CLOCK_MONOTONIC,
		     HRTIMER_MODE_REL_PINNED_SOFT);
	tcp_sk(sk)->compressed_ack_timer.function = tcp_compressed_ack_kick;
}