void tcp_set_keepalive(struct sock *sk, int val)
{
	if ((1 << sk->sk_state) & (TCPF_CLOSE | TCPF_LISTEN))
		return;

	if (val && !sock_flag(sk, SOCK_KEEPOPEN))
		inet_csk_reset_keepalive_timer(sk, keepalive_time_when(tcp_sk(sk)));
	else if (!val)
		inet_csk_delete_keepalive_timer(sk);
}