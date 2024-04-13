struct dst_entry *icmp6_dst_alloc(struct net_device *dev,
				  struct flowi6 *fl6)
{
	struct dst_entry *dst;
	struct rt6_info *rt;
	struct inet6_dev *idev = in6_dev_get(dev);
	struct net *net = dev_net(dev);

	if (unlikely(!idev))
		return ERR_PTR(-ENODEV);

	rt = ip6_dst_alloc(net, dev, 0, NULL);
	if (unlikely(!rt)) {
		in6_dev_put(idev);
		dst = ERR_PTR(-ENOMEM);
		goto out;
	}

	rt->dst.flags |= DST_HOST;
	rt->dst.output  = ip6_output;
	atomic_set(&rt->dst.__refcnt, 1);
	rt->rt6i_gateway  = fl6->daddr;
	rt->rt6i_dst.addr = fl6->daddr;
	rt->rt6i_dst.plen = 128;
	rt->rt6i_idev     = idev;
	dst_metric_set(&rt->dst, RTAX_HOPLIMIT, 0);

	spin_lock_bh(&icmp6_dst_lock);
	rt->dst.next = icmp6_dst_gc_list;
	icmp6_dst_gc_list = &rt->dst;
	spin_unlock_bh(&icmp6_dst_lock);

	fib6_force_start_gc(net);

	dst = xfrm_lookup(net, &rt->dst, flowi6_to_flowi(fl6), NULL, 0);

out:
	return dst;
}