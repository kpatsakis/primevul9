mISDN_sock_sendmsg(struct socket *sock, struct msghdr *msg, size_t len)
{
	struct sock		*sk = sock->sk;
	struct sk_buff		*skb;
	int			err = -ENOMEM;

	if (*debug & DEBUG_SOCKET)
		printk(KERN_DEBUG "%s: len %d flags %x ch %d proto %x\n",
		       __func__, (int)len, msg->msg_flags, _pms(sk)->ch.nr,
		       sk->sk_protocol);

	if (msg->msg_flags & MSG_OOB)
		return -EOPNOTSUPP;

	if (msg->msg_flags & ~(MSG_DONTWAIT | MSG_NOSIGNAL | MSG_ERRQUEUE))
		return -EINVAL;

	if (len < MISDN_HEADER_LEN)
		return -EINVAL;

	if (sk->sk_state != MISDN_BOUND)
		return -EBADFD;

	lock_sock(sk);

	skb = _l2_alloc_skb(len, GFP_KERNEL);
	if (!skb)
		goto done;

	if (memcpy_from_msg(skb_put(skb, len), msg, len)) {
		err = -EFAULT;
		goto done;
	}

	memcpy(mISDN_HEAD_P(skb), skb->data, MISDN_HEADER_LEN);
	skb_pull(skb, MISDN_HEADER_LEN);

	if (msg->msg_namelen >= sizeof(struct sockaddr_mISDN)) {
		/* if we have a address, we use it */
		DECLARE_SOCKADDR(struct sockaddr_mISDN *, maddr, msg->msg_name);
		mISDN_HEAD_ID(skb) = maddr->channel;
	} else { /* use default for L2 messages */
		if ((sk->sk_protocol == ISDN_P_LAPD_TE) ||
		    (sk->sk_protocol == ISDN_P_LAPD_NT))
			mISDN_HEAD_ID(skb) = _pms(sk)->ch.nr;
	}

	if (*debug & DEBUG_SOCKET)
		printk(KERN_DEBUG "%s: ID:%x\n",
		       __func__, mISDN_HEAD_ID(skb));

	err = -ENODEV;
	if (!_pms(sk)->ch.peer)
		goto done;
	err = _pms(sk)->ch.recv(_pms(sk)->ch.peer, skb);
	if (err)
		goto done;
	else {
		skb = NULL;
		err = len;
	}

done:
	kfree_skb(skb);
	release_sock(sk);
	return err;
}