static int netlink_sendmsg(struct socket *sock, struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	struct netlink_sock *nlk = nlk_sk(sk);
	DECLARE_SOCKADDR(struct sockaddr_nl *, addr, msg->msg_name);
	u32 dst_portid;
	u32 dst_group;
	struct sk_buff *skb;
	int err;
	struct scm_cookie scm;
	u32 netlink_skb_flags = 0;

	if (msg->msg_flags&MSG_OOB)
		return -EOPNOTSUPP;

	err = scm_send(sock, msg, &scm, true);
	if (err < 0)
		return err;

	if (msg->msg_namelen) {
		err = -EINVAL;
		if (addr->nl_family != AF_NETLINK)
			goto out;
		dst_portid = addr->nl_pid;
		dst_group = ffs(addr->nl_groups);
		err =  -EPERM;
		if ((dst_group || dst_portid) &&
		    !netlink_allowed(sock, NL_CFG_F_NONROOT_SEND))
			goto out;
		netlink_skb_flags |= NETLINK_SKB_DST;
	} else {
		dst_portid = nlk->dst_portid;
		dst_group = nlk->dst_group;
	}

	if (!nlk->bound) {
		err = netlink_autobind(sock);
		if (err)
			goto out;
	} else {
		/* Ensure nlk is hashed and visible. */
		smp_rmb();
	}

	err = -EMSGSIZE;
	if (len > sk->sk_sndbuf - 32)
		goto out;
	err = -ENOBUFS;
	skb = netlink_alloc_large_skb(len, dst_group);
	if (skb == NULL)
		goto out;

	NETLINK_CB(skb).portid	= nlk->portid;
	NETLINK_CB(skb).dst_group = dst_group;
	NETLINK_CB(skb).creds	= scm.creds;
	NETLINK_CB(skb).flags	= netlink_skb_flags;

	err = -EFAULT;
	if (memcpy_from_msg(skb_put(skb, len), msg, len)) {
		kfree_skb(skb);
		goto out;
	}

	err = security_netlink_send(sk, skb);
	if (err) {
		kfree_skb(skb);
		goto out;
	}

	if (dst_group) {
		refcount_inc(&skb->users);
		netlink_broadcast(sk, skb, dst_portid, dst_group, GFP_KERNEL);
	}
	err = netlink_unicast(sk, skb, dst_portid, msg->msg_flags&MSG_DONTWAIT);

out:
	scm_destroy(&scm);
	return err;
}