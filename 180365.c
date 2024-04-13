void dccp_done(struct sock *sk)
{
	dccp_set_state(sk, DCCP_CLOSED);
	dccp_clear_xmit_timers(sk);

	sk->sk_shutdown = SHUTDOWN_MASK;

	if (!sock_flag(sk, SOCK_DEAD))
		sk->sk_state_change(sk);
	else
		inet_csk_destroy_sock(sk);
}