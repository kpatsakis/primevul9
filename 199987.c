int dccp_disconnect(struct sock *sk, int flags)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	struct inet_sock *inet = inet_sk(sk);
	int err = 0;
	const int old_state = sk->sk_state;

	if (old_state != DCCP_CLOSED)
		dccp_set_state(sk, DCCP_CLOSED);

	/* ABORT function of RFC793 */
	if (old_state == DCCP_LISTEN) {
		inet_csk_listen_stop(sk);
	/* FIXME: do the active reset thing */
	} else if (old_state == DCCP_REQUESTING)
		sk->sk_err = ECONNRESET;

	dccp_clear_xmit_timers(sk);
	__skb_queue_purge(&sk->sk_receive_queue);
	if (sk->sk_send_head != NULL) {
		__kfree_skb(sk->sk_send_head);
		sk->sk_send_head = NULL;
	}

	inet->dport = 0;

	if (!(sk->sk_userlocks & SOCK_BINDADDR_LOCK))
		inet_reset_saddr(sk);

	sk->sk_shutdown = 0;
	sock_reset_flag(sk, SOCK_DONE);

	icsk->icsk_backoff = 0;
	inet_csk_delack_init(sk);
	__sk_dst_reset(sk);

	BUG_TRAP(!inet->num || icsk->icsk_bind_hash);

	sk->sk_error_report(sk);
	return err;
}