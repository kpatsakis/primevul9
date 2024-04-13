struct ctl_table * __net_init ipv6_route_sysctl_init(struct net *net)
{
	struct ctl_table *table;

	table = kmemdup(ipv6_route_table_template,
			sizeof(ipv6_route_table_template),
			GFP_KERNEL);

	if (table) {
		table[0].data = &net->ipv6.sysctl.flush_delay;
		table[0].extra1 = net;
		table[1].data = &net->ipv6.ip6_dst_ops.gc_thresh;
		table[2].data = &net->ipv6.sysctl.ip6_rt_max_size;
		table[3].data = &net->ipv6.sysctl.ip6_rt_gc_min_interval;
		table[4].data = &net->ipv6.sysctl.ip6_rt_gc_timeout;
		table[5].data = &net->ipv6.sysctl.ip6_rt_gc_interval;
		table[6].data = &net->ipv6.sysctl.ip6_rt_gc_elasticity;
		table[7].data = &net->ipv6.sysctl.ip6_rt_mtu_expires;
		table[8].data = &net->ipv6.sysctl.ip6_rt_min_advmss;
		table[9].data = &net->ipv6.sysctl.ip6_rt_gc_min_interval;

		/* Don't export sysctls to unprivileged users */
		if (net->user_ns != &init_user_ns)
			table[0].procname = NULL;
	}

	return table;
}