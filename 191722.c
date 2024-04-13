int netlink_broadcast_filtered(struct sock *ssk, struct sk_buff *skb, u32 portid,
	u32 group, gfp_t allocation,
	int (*filter)(struct sock *dsk, struct sk_buff *skb, void *data),
	void *filter_data)
{
	struct net *net = sock_net(ssk);
	struct netlink_broadcast_data info;
	struct sock *sk;

	skb = netlink_trim(skb, allocation);

	info.exclude_sk = ssk;
	info.net = net;
	info.portid = portid;
	info.group = group;
	info.failure = 0;
	info.delivery_failure = 0;
	info.congested = 0;
	info.delivered = 0;
	info.allocation = allocation;
	info.skb = skb;
	info.skb2 = NULL;
	info.tx_filter = filter;
	info.tx_data = filter_data;

	/* While we sleep in clone, do not allow to change socket list */

	netlink_lock_table();

	sk_for_each_bound(sk, &nl_table[ssk->sk_protocol].mc_list)
		do_one_broadcast(sk, &info);

	consume_skb(skb);

	netlink_unlock_table();

	if (info.delivery_failure) {
		kfree_skb(info.skb2);
		return -ENOBUFS;
	}
	consume_skb(info.skb2);

	if (info.delivered) {
		if (info.congested && gfpflags_allow_blocking(allocation))
			yield();
		return 0;
	}
	return -ESRCH;
}