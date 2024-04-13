void dccp_close(struct sock *sk, long timeout)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct sk_buff *skb;
	int state;

	lock_sock(sk);

	sk->sk_shutdown = SHUTDOWN_MASK;

	if (sk->sk_state == DCCP_LISTEN) {
		dccp_set_state(sk, DCCP_CLOSED);

		/* Special case. */
		inet_csk_listen_stop(sk);

		goto adjudge_to_death;
	}

	sk_stop_timer(sk, &dp->dccps_xmit_timer);

	/*
	 * We need to flush the recv. buffs.  We do this only on the
	 * descriptor close, not protocol-sourced closes, because the
	  *reader process may not have drained the data yet!
	 */
	/* FIXME: check for unread data */
	while ((skb = __skb_dequeue(&sk->sk_receive_queue)) != NULL) {
		__kfree_skb(skb);
	}

	if (sock_flag(sk, SOCK_LINGER) && !sk->sk_lingertime) {
		/* Check zero linger _after_ checking for unread data. */
		sk->sk_prot->disconnect(sk, 0);
	} else if (dccp_close_state(sk)) {
		dccp_send_close(sk, 1);
	}

	sk_stream_wait_close(sk, timeout);

adjudge_to_death:
	state = sk->sk_state;
	sock_hold(sk);
	sock_orphan(sk);
	atomic_inc(sk->sk_prot->orphan_count);

	/*
	 * It is the last release_sock in its life. It will remove backlog.
	 */
	release_sock(sk);
	/*
	 * Now socket is owned by kernel and we acquire BH lock
	 * to finish close. No need to check for user refs.
	 */
	local_bh_disable();
	bh_lock_sock(sk);
	BUG_TRAP(!sock_owned_by_user(sk));

	/* Have we already been destroyed by a softirq or backlog? */
	if (state != DCCP_CLOSED && sk->sk_state == DCCP_CLOSED)
		goto out;

	/*
	 * The last release_sock may have processed the CLOSE or RESET
	 * packet moving sock to CLOSED state, if not we have to fire
	 * the CLOSE/CLOSEREQ retransmission timer, see "8.3. Termination"
	 * in draft-ietf-dccp-spec-11. -acme
	 */
	if (sk->sk_state == DCCP_CLOSING) {
		/* FIXME: should start at 2 * RTT */
		/* Timer for repeating the CLOSE/CLOSEREQ until an answer. */
		inet_csk_reset_xmit_timer(sk, ICSK_TIME_RETRANS,
					  inet_csk(sk)->icsk_rto,
					  DCCP_RTO_MAX);
#if 0
		/* Yeah, we should use sk->sk_prot->orphan_count, etc */
		dccp_set_state(sk, DCCP_CLOSED);
#endif
	}

	if (sk->sk_state == DCCP_CLOSED)
		inet_csk_destroy_sock(sk);

	/* Otherwise, socket is reprieved until protocol close. */

out:
	bh_unlock_sock(sk);
	local_bh_enable();
	sock_put(sk);
}