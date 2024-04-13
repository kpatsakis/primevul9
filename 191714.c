static void do_one_broadcast(struct sock *sk,
				    struct netlink_broadcast_data *p)
{
	struct netlink_sock *nlk = nlk_sk(sk);
	int val;

	if (p->exclude_sk == sk)
		return;

	if (nlk->portid == p->portid || p->group - 1 >= nlk->ngroups ||
	    !test_bit(p->group - 1, nlk->groups))
		return;

	if (!net_eq(sock_net(sk), p->net)) {
		if (!(nlk->flags & NETLINK_F_LISTEN_ALL_NSID))
			return;

		if (!peernet_has_id(sock_net(sk), p->net))
			return;

		if (!file_ns_capable(sk->sk_socket->file, p->net->user_ns,
				     CAP_NET_BROADCAST))
			return;
	}

	if (p->failure) {
		netlink_overrun(sk);
		return;
	}

	sock_hold(sk);
	if (p->skb2 == NULL) {
		if (skb_shared(p->skb)) {
			p->skb2 = skb_clone(p->skb, p->allocation);
		} else {
			p->skb2 = skb_get(p->skb);
			/*
			 * skb ownership may have been set when
			 * delivered to a previous socket.
			 */
			skb_orphan(p->skb2);
		}
	}
	if (p->skb2 == NULL) {
		netlink_overrun(sk);
		/* Clone failed. Notify ALL listeners. */
		p->failure = 1;
		if (nlk->flags & NETLINK_F_BROADCAST_SEND_ERROR)
			p->delivery_failure = 1;
		goto out;
	}
	if (p->tx_filter && p->tx_filter(sk, p->skb2, p->tx_data)) {
		kfree_skb(p->skb2);
		p->skb2 = NULL;
		goto out;
	}
	if (sk_filter(sk, p->skb2)) {
		kfree_skb(p->skb2);
		p->skb2 = NULL;
		goto out;
	}
	NETLINK_CB(p->skb2).nsid = peernet2id(sock_net(sk), p->net);
	if (NETLINK_CB(p->skb2).nsid != NETNSA_NSID_NOT_ASSIGNED)
		NETLINK_CB(p->skb2).nsid_is_set = true;
	val = netlink_broadcast_deliver(sk, p->skb2);
	if (val < 0) {
		netlink_overrun(sk);
		if (nlk->flags & NETLINK_F_BROADCAST_SEND_ERROR)
			p->delivery_failure = 1;
	} else {
		p->congested |= val;
		p->delivered = 1;
		p->skb2 = NULL;
	}
out:
	sock_put(sk);
}