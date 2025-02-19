static int llc_ui_recvmsg(struct socket *sock, struct msghdr *msg, size_t len,
			  int flags)
{
	DECLARE_SOCKADDR(struct sockaddr_llc *, uaddr, msg->msg_name);
	const int nonblock = flags & MSG_DONTWAIT;
	struct sk_buff *skb = NULL;
	struct sock *sk = sock->sk;
	struct llc_sock *llc = llc_sk(sk);
	unsigned long cpu_flags;
	size_t copied = 0;
	u32 peek_seq = 0;
	u32 *seq, skb_len;
	unsigned long used;
	int target;	/* Read at least this many bytes */
	long timeo;

	lock_sock(sk);
	copied = -ENOTCONN;
	if (unlikely(sk->sk_type == SOCK_STREAM && sk->sk_state == TCP_LISTEN))
		goto out;

	timeo = sock_rcvtimeo(sk, nonblock);

	seq = &llc->copied_seq;
	if (flags & MSG_PEEK) {
		peek_seq = llc->copied_seq;
		seq = &peek_seq;
	}

	target = sock_rcvlowat(sk, flags & MSG_WAITALL, len);
	copied = 0;

	do {
		u32 offset;

		/*
		 * We need to check signals first, to get correct SIGURG
		 * handling. FIXME: Need to check this doesn't impact 1003.1g
		 * and move it down to the bottom of the loop
		 */
		if (signal_pending(current)) {
			if (copied)
				break;
			copied = timeo ? sock_intr_errno(timeo) : -EAGAIN;
			break;
		}

		/* Next get a buffer. */

		skb = skb_peek(&sk->sk_receive_queue);
		if (skb) {
			offset = *seq;
			goto found_ok_skb;
		}
		/* Well, if we have backlog, try to process it now yet. */

		if (copied >= target && !sk->sk_backlog.tail)
			break;

		if (copied) {
			if (sk->sk_err ||
			    sk->sk_state == TCP_CLOSE ||
			    (sk->sk_shutdown & RCV_SHUTDOWN) ||
			    !timeo ||
			    (flags & MSG_PEEK))
				break;
		} else {
			if (sock_flag(sk, SOCK_DONE))
				break;

			if (sk->sk_err) {
				copied = sock_error(sk);
				break;
			}
			if (sk->sk_shutdown & RCV_SHUTDOWN)
				break;

			if (sk->sk_type == SOCK_STREAM && sk->sk_state == TCP_CLOSE) {
				if (!sock_flag(sk, SOCK_DONE)) {
					/*
					 * This occurs when user tries to read
					 * from never connected socket.
					 */
					copied = -ENOTCONN;
					break;
				}
				break;
			}
			if (!timeo) {
				copied = -EAGAIN;
				break;
			}
		}

		if (copied >= target) { /* Do not sleep, just process backlog. */
			release_sock(sk);
			lock_sock(sk);
		} else
			sk_wait_data(sk, &timeo, NULL);

		if ((flags & MSG_PEEK) && peek_seq != llc->copied_seq) {
			net_dbg_ratelimited("LLC(%s:%d): Application bug, race in MSG_PEEK\n",
					    current->comm,
					    task_pid_nr(current));
			peek_seq = llc->copied_seq;
		}
		continue;
	found_ok_skb:
		skb_len = skb->len;
		/* Ok so how much can we use? */
		used = skb->len - offset;
		if (len < used)
			used = len;

		if (!(flags & MSG_TRUNC)) {
			int rc = skb_copy_datagram_msg(skb, offset, msg, used);
			if (rc) {
				/* Exception. Bailout! */
				if (!copied)
					copied = -EFAULT;
				break;
			}
		}

		*seq += used;
		copied += used;
		len -= used;

		/* For non stream protcols we get one packet per recvmsg call */
		if (sk->sk_type != SOCK_STREAM)
			goto copy_uaddr;

		if (!(flags & MSG_PEEK)) {
			spin_lock_irqsave(&sk->sk_receive_queue.lock, cpu_flags);
			sk_eat_skb(sk, skb);
			spin_unlock_irqrestore(&sk->sk_receive_queue.lock, cpu_flags);
			*seq = 0;
		}

		/* Partial read */
		if (used + offset < skb_len)
			continue;
	} while (len > 0);

out:
	release_sock(sk);
	return copied;
copy_uaddr:
	if (uaddr != NULL && skb != NULL) {
		memcpy(uaddr, llc_ui_skb_cb(skb), sizeof(*uaddr));
		msg->msg_namelen = sizeof(*uaddr);
	}
	if (llc_sk(sk)->cmsg_flags)
		llc_cmsg_rcv(msg, skb);

	if (!(flags & MSG_PEEK)) {
		spin_lock_irqsave(&sk->sk_receive_queue.lock, cpu_flags);
		sk_eat_skb(sk, skb);
		spin_unlock_irqrestore(&sk->sk_receive_queue.lock, cpu_flags);
		*seq = 0;
	}

	goto out;
}