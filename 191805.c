static int netlink_recvmsg(struct socket *sock, struct msghdr *msg, size_t len,
			   int flags)
{
	struct scm_cookie scm;
	struct sock *sk = sock->sk;
	struct netlink_sock *nlk = nlk_sk(sk);
	int noblock = flags&MSG_DONTWAIT;
	size_t copied;
	struct sk_buff *skb, *data_skb;
	int err, ret;

	if (flags&MSG_OOB)
		return -EOPNOTSUPP;

	copied = 0;

	skb = skb_recv_datagram(sk, flags, noblock, &err);
	if (skb == NULL)
		goto out;

	data_skb = skb;

#ifdef CONFIG_COMPAT_NETLINK_MESSAGES
	if (unlikely(skb_shinfo(skb)->frag_list)) {
		/*
		 * If this skb has a frag_list, then here that means that we
		 * will have to use the frag_list skb's data for compat tasks
		 * and the regular skb's data for normal (non-compat) tasks.
		 *
		 * If we need to send the compat skb, assign it to the
		 * 'data_skb' variable so that it will be used below for data
		 * copying. We keep 'skb' for everything else, including
		 * freeing both later.
		 */
		if (flags & MSG_CMSG_COMPAT)
			data_skb = skb_shinfo(skb)->frag_list;
	}
#endif

	/* Record the max length of recvmsg() calls for future allocations */
	nlk->max_recvmsg_len = max(nlk->max_recvmsg_len, len);
	nlk->max_recvmsg_len = min_t(size_t, nlk->max_recvmsg_len,
				     SKB_WITH_OVERHEAD(32768));

	copied = data_skb->len;
	if (len < copied) {
		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}

	skb_reset_transport_header(data_skb);
	err = skb_copy_datagram_msg(data_skb, 0, msg, copied);

	if (msg->msg_name) {
		DECLARE_SOCKADDR(struct sockaddr_nl *, addr, msg->msg_name);
		addr->nl_family = AF_NETLINK;
		addr->nl_pad    = 0;
		addr->nl_pid	= NETLINK_CB(skb).portid;
		addr->nl_groups	= netlink_group_mask(NETLINK_CB(skb).dst_group);
		msg->msg_namelen = sizeof(*addr);
	}

	if (nlk->flags & NETLINK_F_RECV_PKTINFO)
		netlink_cmsg_recv_pktinfo(msg, skb);
	if (nlk->flags & NETLINK_F_LISTEN_ALL_NSID)
		netlink_cmsg_listen_all_nsid(sk, msg, skb);

	memset(&scm, 0, sizeof(scm));
	scm.creds = *NETLINK_CREDS(skb);
	if (flags & MSG_TRUNC)
		copied = data_skb->len;

	skb_free_datagram(sk, skb);

	if (nlk->cb_running &&
	    atomic_read(&sk->sk_rmem_alloc) <= sk->sk_rcvbuf / 2) {
		ret = netlink_dump(sk);
		if (ret) {
			sk->sk_err = -ret;
			sk->sk_error_report(sk);
		}
	}

	scm_recv(sock, msg, &scm, flags);
out:
	netlink_rcv_wake(sk);
	return err ? : copied;
}