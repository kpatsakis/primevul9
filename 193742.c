static inline struct rt6_info *ip6_dst_alloc(struct net *net,
					     struct net_device *dev,
					     int flags,
					     struct fib6_table *table)
{
	struct rt6_info *rt = dst_alloc(&net->ipv6.ip6_dst_ops, dev,
					0, DST_OBSOLETE_FORCE_CHK, flags);

	if (rt) {
		struct dst_entry *dst = &rt->dst;

		memset(dst + 1, 0, sizeof(*rt) - sizeof(*dst));
		rt6_init_peer(rt, table ? &table->tb6_peers : net->ipv6.peers);
		rt->rt6i_genid = rt_genid_ipv6(net);
		INIT_LIST_HEAD(&rt->rt6i_siblings);
	}
	return rt;
}