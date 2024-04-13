static void tcp_write_err(struct sock *sk)
{
	sk->sk_err = sk->sk_err_soft ? : ETIMEDOUT;
	sk->sk_error_report(sk);

	tcp_write_queue_purge(sk);
	tcp_done(sk);
	__NET_INC_STATS(sock_net(sk), LINUX_MIB_TCPABORTONTIMEOUT);
}