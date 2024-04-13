void dccp_send_delayed_ack(struct sock *sk)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	/*
	 * FIXME: tune this timer. elapsed time fixes the skew, so no problem
	 * with using 2s, and active senders also piggyback the ACK into a
	 * DATAACK packet, so this is really for quiescent senders.
	 */
	unsigned long timeout = jiffies + 2 * HZ;

	/* Use new timeout only if there wasn't a older one earlier. */
	if (icsk->icsk_ack.pending & ICSK_ACK_TIMER) {
		/* If delack timer was blocked or is about to expire,
		 * send ACK now.
		 *
		 * FIXME: check the "about to expire" part
		 */
		if (icsk->icsk_ack.blocked) {
			dccp_send_ack(sk);
			return;
		}

		if (!time_before(timeout, icsk->icsk_ack.timeout))
			timeout = icsk->icsk_ack.timeout;
	}
	icsk->icsk_ack.pending |= ICSK_ACK_SCHED | ICSK_ACK_TIMER;
	icsk->icsk_ack.timeout = timeout;
	sk_reset_timer(sk, &icsk->icsk_delack_timer, timeout);
}