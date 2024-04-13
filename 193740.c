static int ip6_dst_gc(struct dst_ops *ops)
{
	struct net *net = container_of(ops, struct net, ipv6.ip6_dst_ops);
	int rt_min_interval = net->ipv6.sysctl.ip6_rt_gc_min_interval;
	int rt_max_size = net->ipv6.sysctl.ip6_rt_max_size;
	int rt_elasticity = net->ipv6.sysctl.ip6_rt_gc_elasticity;
	int rt_gc_timeout = net->ipv6.sysctl.ip6_rt_gc_timeout;
	unsigned long rt_last_gc = net->ipv6.ip6_rt_last_gc;
	int entries;

	entries = dst_entries_get_fast(ops);
	if (time_after(rt_last_gc + rt_min_interval, jiffies) &&
	    entries <= rt_max_size)
		goto out;

	net->ipv6.ip6_rt_gc_expire++;
	fib6_run_gc(net->ipv6.ip6_rt_gc_expire, net, entries > rt_max_size);
	entries = dst_entries_get_slow(ops);
	if (entries < ops->gc_thresh)
		net->ipv6.ip6_rt_gc_expire = rt_gc_timeout>>1;
out:
	net->ipv6.ip6_rt_gc_expire -= net->ipv6.ip6_rt_gc_expire>>rt_elasticity;
	return entries > rt_max_size;
}