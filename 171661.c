static void __sk_free(struct sock *sk)
{
	if (unlikely(sock_diag_has_destroy_listeners(sk) && sk->sk_net_refcnt))
		sock_diag_broadcast_destroy(sk);
	else
		sk_destruct(sk);
}