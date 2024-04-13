static int inet6_dump_addr(struct sk_buff *skb, struct netlink_callback *cb,
			   enum addr_type_t type)
{
	struct net *net = sock_net(skb->sk);
	int h, s_h;
	int idx, ip_idx;
	int s_idx, s_ip_idx;
	struct net_device *dev;
	struct inet6_dev *idev;
	struct hlist_head *head;

	s_h = cb->args[0];
	s_idx = idx = cb->args[1];
	s_ip_idx = ip_idx = cb->args[2];

	rcu_read_lock();
	cb->seq = atomic_read(&net->ipv6.dev_addr_genid) ^ net->dev_base_seq;
	for (h = s_h; h < NETDEV_HASHENTRIES; h++, s_idx = 0) {
		idx = 0;
		head = &net->dev_index_head[h];
		hlist_for_each_entry_rcu(dev, head, index_hlist) {
			if (idx < s_idx)
				goto cont;
			if (h > s_h || idx > s_idx)
				s_ip_idx = 0;
			ip_idx = 0;
			idev = __in6_dev_get(dev);
			if (!idev)
				goto cont;

			if (in6_dump_addrs(idev, skb, cb, type,
					   s_ip_idx, &ip_idx) <= 0)
				goto done;
cont:
			idx++;
		}
	}
done:
	rcu_read_unlock();
	cb->args[0] = h;
	cb->args[1] = idx;
	cb->args[2] = ip_idx;

	return skb->len;
}