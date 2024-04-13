static int inet6_dump_ifinfo(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	int h, s_h;
	int idx = 0, s_idx;
	struct net_device *dev;
	struct inet6_dev *idev;
	struct hlist_head *head;

	s_h = cb->args[0];
	s_idx = cb->args[1];

	rcu_read_lock();
	for (h = s_h; h < NETDEV_HASHENTRIES; h++, s_idx = 0) {
		idx = 0;
		head = &net->dev_index_head[h];
		hlist_for_each_entry_rcu(dev, head, index_hlist) {
			if (idx < s_idx)
				goto cont;
			idev = __in6_dev_get(dev);
			if (!idev)
				goto cont;
			if (inet6_fill_ifinfo(skb, idev,
					      NETLINK_CB(cb->skb).portid,
					      cb->nlh->nlmsg_seq,
					      RTM_NEWLINK, NLM_F_MULTI) <= 0)
				goto out;
cont:
			idx++;
		}
	}
out:
	rcu_read_unlock();
	cb->args[1] = idx;
	cb->args[0] = h;

	return skb->len;
}