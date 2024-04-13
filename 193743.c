struct dst_entry *ip6_blackhole_route(struct net *net, struct dst_entry *dst_orig)
{
	struct rt6_info *rt, *ort = (struct rt6_info *) dst_orig;
	struct dst_entry *new = NULL;

	rt = dst_alloc(&ip6_dst_blackhole_ops, ort->dst.dev, 1, DST_OBSOLETE_NONE, 0);
	if (rt) {
		new = &rt->dst;

		memset(new + 1, 0, sizeof(*rt) - sizeof(*new));
		rt6_init_peer(rt, net->ipv6.peers);

		new->__use = 1;
		new->input = dst_discard;
		new->output = dst_discard;

		if (dst_metrics_read_only(&ort->dst))
			new->_metrics = ort->dst._metrics;
		else
			dst_copy_metrics(new, &ort->dst);
		rt->rt6i_idev = ort->rt6i_idev;
		if (rt->rt6i_idev)
			in6_dev_hold(rt->rt6i_idev);

		rt->rt6i_gateway = ort->rt6i_gateway;
		rt->rt6i_flags = ort->rt6i_flags;
		rt->rt6i_metric = 0;

		memcpy(&rt->rt6i_dst, &ort->rt6i_dst, sizeof(struct rt6key));
#ifdef CONFIG_IPV6_SUBTREES
		memcpy(&rt->rt6i_src, &ort->rt6i_src, sizeof(struct rt6key));
#endif

		dst_free(new);
	}

	dst_release(dst_orig);
	return new ? new : ERR_PTR(-ENOMEM);
}