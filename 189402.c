static int ax25_sendmsg(struct socket *sock, struct msghdr *msg, size_t len)
{
	DECLARE_SOCKADDR(struct sockaddr_ax25 *, usax, msg->msg_name);
	struct sock *sk = sock->sk;
	struct sockaddr_ax25 sax;
	struct sk_buff *skb;
	ax25_digi dtmp, *dp;
	ax25_cb *ax25;
	size_t size;
	int lv, err, addr_len = msg->msg_namelen;

	if (msg->msg_flags & ~(MSG_DONTWAIT|MSG_EOR|MSG_CMSG_COMPAT))
		return -EINVAL;

	lock_sock(sk);
	ax25 = sk_to_ax25(sk);

	if (sock_flag(sk, SOCK_ZAPPED)) {
		err = -EADDRNOTAVAIL;
		goto out;
	}

	if (sk->sk_shutdown & SEND_SHUTDOWN) {
		send_sig(SIGPIPE, current, 0);
		err = -EPIPE;
		goto out;
	}

	if (ax25->ax25_dev == NULL) {
		err = -ENETUNREACH;
		goto out;
	}

	if (len > ax25->ax25_dev->dev->mtu) {
		err = -EMSGSIZE;
		goto out;
	}

	if (usax != NULL) {
		if (usax->sax25_family != AF_AX25) {
			err = -EINVAL;
			goto out;
		}

		if (addr_len == sizeof(struct sockaddr_ax25))
			/* ax25_sendmsg(): uses obsolete socket structure */
			;
		else if (addr_len != sizeof(struct full_sockaddr_ax25))
			/* support for old structure may go away some time
			 * ax25_sendmsg(): uses old (6 digipeater)
			 * socket structure.
			 */
			if ((addr_len < sizeof(struct sockaddr_ax25) + sizeof(ax25_address) * 6) ||
			    (addr_len > sizeof(struct full_sockaddr_ax25))) {
				err = -EINVAL;
				goto out;
			}


		if (addr_len > sizeof(struct sockaddr_ax25) && usax->sax25_ndigis != 0) {
			int ct           = 0;
			struct full_sockaddr_ax25 *fsa = (struct full_sockaddr_ax25 *)usax;

			/* Valid number of digipeaters ? */
			if (usax->sax25_ndigis < 1 || usax->sax25_ndigis > AX25_MAX_DIGIS) {
				err = -EINVAL;
				goto out;
			}

			dtmp.ndigi      = usax->sax25_ndigis;

			while (ct < usax->sax25_ndigis) {
				dtmp.repeated[ct] = 0;
				dtmp.calls[ct]    = fsa->fsa_digipeater[ct];
				ct++;
			}

			dtmp.lastrepeat = 0;
		}

		sax = *usax;
		if (sk->sk_type == SOCK_SEQPACKET &&
		    ax25cmp(&ax25->dest_addr, &sax.sax25_call)) {
			err = -EISCONN;
			goto out;
		}
		if (usax->sax25_ndigis == 0)
			dp = NULL;
		else
			dp = &dtmp;
	} else {
		/*
		 *	FIXME: 1003.1g - if the socket is like this because
		 *	it has become closed (not started closed) and is VC
		 *	we ought to SIGPIPE, EPIPE
		 */
		if (sk->sk_state != TCP_ESTABLISHED) {
			err = -ENOTCONN;
			goto out;
		}
		sax.sax25_family = AF_AX25;
		sax.sax25_call   = ax25->dest_addr;
		dp = ax25->digipeat;
	}

	/* Build a packet */
	/* Assume the worst case */
	size = len + ax25->ax25_dev->dev->hard_header_len;

	skb = sock_alloc_send_skb(sk, size, msg->msg_flags&MSG_DONTWAIT, &err);
	if (skb == NULL)
		goto out;

	skb_reserve(skb, size - len);

	/* User data follows immediately after the AX.25 data */
	if (memcpy_from_msg(skb_put(skb, len), msg, len)) {
		err = -EFAULT;
		kfree_skb(skb);
		goto out;
	}

	skb_reset_network_header(skb);

	/* Add the PID if one is not supplied by the user in the skb */
	if (!ax25->pidincl)
		*(u8 *)skb_push(skb, 1) = sk->sk_protocol;

	if (sk->sk_type == SOCK_SEQPACKET) {
		/* Connected mode sockets go via the LAPB machine */
		if (sk->sk_state != TCP_ESTABLISHED) {
			kfree_skb(skb);
			err = -ENOTCONN;
			goto out;
		}

		/* Shove it onto the queue and kick */
		ax25_output(ax25, ax25->paclen, skb);

		err = len;
		goto out;
	}

	skb_push(skb, 1 + ax25_addr_size(dp));

	/* Building AX.25 Header */

	/* Build an AX.25 header */
	lv = ax25_addr_build(skb->data, &ax25->source_addr, &sax.sax25_call,
			     dp, AX25_COMMAND, AX25_MODULUS);

	skb_set_transport_header(skb, lv);

	*skb_transport_header(skb) = AX25_UI;

	/* Datagram frames go straight out of the door as UI */
	ax25_queue_xmit(skb, ax25->ax25_dev->dev);

	err = len;

out:
	release_sock(sk);

	return err;
}