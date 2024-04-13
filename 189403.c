static int ax25_accept(struct socket *sock, struct socket *newsock, int flags,
		       bool kern)
{
	struct sk_buff *skb;
	struct sock *newsk;
	DEFINE_WAIT(wait);
	struct sock *sk;
	int err = 0;

	if (sock->state != SS_UNCONNECTED)
		return -EINVAL;

	if ((sk = sock->sk) == NULL)
		return -EINVAL;

	lock_sock(sk);
	if (sk->sk_type != SOCK_SEQPACKET) {
		err = -EOPNOTSUPP;
		goto out;
	}

	if (sk->sk_state != TCP_LISTEN) {
		err = -EINVAL;
		goto out;
	}

	/*
	 *	The read queue this time is holding sockets ready to use
	 *	hooked into the SABM we saved
	 */
	for (;;) {
		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		skb = skb_dequeue(&sk->sk_receive_queue);
		if (skb)
			break;

		if (flags & O_NONBLOCK) {
			err = -EWOULDBLOCK;
			break;
		}
		if (!signal_pending(current)) {
			release_sock(sk);
			schedule();
			lock_sock(sk);
			continue;
		}
		err = -ERESTARTSYS;
		break;
	}
	finish_wait(sk_sleep(sk), &wait);

	if (err)
		goto out;

	newsk		 = skb->sk;
	sock_graft(newsk, newsock);

	/* Now attach up the new socket */
	kfree_skb(skb);
	sk->sk_ack_backlog--;
	newsock->state = SS_CONNECTED;

out:
	release_sock(sk);

	return err;
}