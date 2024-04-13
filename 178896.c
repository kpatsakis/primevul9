static void pptp_sock_destruct(struct sock *sk)
{
	if (!(sk->sk_state & PPPOX_DEAD)) {
		del_chan(pppox_sk(sk));
		pppox_unbind_sock(sk);
	}
	skb_queue_purge(&sk->sk_receive_queue);
}