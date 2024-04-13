static int irda_accept(struct socket *sock, struct socket *newsock, int flags,
		       bool kern)
{
	struct sock *sk = sock->sk;
	struct irda_sock *new, *self = irda_sk(sk);
	struct sock *newsk;
	struct sk_buff *skb = NULL;
	int err;

	err = irda_create(sock_net(sk), newsock, sk->sk_protocol, kern);
	if (err)
		return err;

	err = -EINVAL;

	lock_sock(sk);
	if (sock->state != SS_UNCONNECTED)
		goto out;

	err = -EOPNOTSUPP;
	if ((sk->sk_type != SOCK_STREAM) && (sk->sk_type != SOCK_SEQPACKET) &&
	    (sk->sk_type != SOCK_DGRAM))
		goto out;

	err = -EINVAL;
	if (sk->sk_state != TCP_LISTEN)
		goto out;

	/*
	 *	The read queue this time is holding sockets ready to use
	 *	hooked into the SABM we saved
	 */

	/*
	 * We can perform the accept only if there is incoming data
	 * on the listening socket.
	 * So, we will block the caller until we receive any data.
	 * If the caller was waiting on select() or poll() before
	 * calling us, the data is waiting for us ;-)
	 * Jean II
	 */
	while (1) {
		skb = skb_dequeue(&sk->sk_receive_queue);
		if (skb)
			break;

		/* Non blocking operation */
		err = -EWOULDBLOCK;
		if (flags & O_NONBLOCK)
			goto out;

		err = wait_event_interruptible(*(sk_sleep(sk)),
					skb_peek(&sk->sk_receive_queue));
		if (err)
			goto out;
	}

	newsk = newsock->sk;
	err = -EIO;
	if (newsk == NULL)
		goto out;

	newsk->sk_state = TCP_ESTABLISHED;

	new = irda_sk(newsk);

	/* Now attach up the new socket */
	new->tsap = irttp_dup(self->tsap, new);
	err = -EPERM; /* value does not seem to make sense. -arnd */
	if (!new->tsap) {
		pr_debug("%s(), dup failed!\n", __func__);
		goto out;
	}

	new->stsap_sel = new->tsap->stsap_sel;
	new->dtsap_sel = new->tsap->dtsap_sel;
	new->saddr = irttp_get_saddr(new->tsap);
	new->daddr = irttp_get_daddr(new->tsap);

	new->max_sdu_size_tx = self->max_sdu_size_tx;
	new->max_sdu_size_rx = self->max_sdu_size_rx;
	new->max_data_size   = self->max_data_size;
	new->max_header_size = self->max_header_size;

	memcpy(&new->qos_tx, &self->qos_tx, sizeof(struct qos_info));

	/* Clean up the original one to keep it in listen state */
	irttp_listen(self->tsap);

	sk->sk_ack_backlog--;

	newsock->state = SS_CONNECTED;

	irda_connect_response(new);
	err = 0;
out:
	kfree_skb(skb);
	release_sock(sk);
	return err;
}