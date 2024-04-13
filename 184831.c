static int __net_init ip4_frags_ns_ctl_register(struct net *net)
{
	struct ctl_table *table;
	struct ctl_table_header *hdr;

	table = ip4_frags_ns_ctl_table;
	if (!net_eq(net, &init_net)) {
		table = kmemdup(table, sizeof(ip4_frags_ns_ctl_table), GFP_KERNEL);
		if (!table)
			goto err_alloc;

		table[0].data = &net->ipv4.frags.high_thresh;
		table[0].extra1 = &net->ipv4.frags.low_thresh;
		table[0].extra2 = &init_net.ipv4.frags.high_thresh;
		table[1].data = &net->ipv4.frags.low_thresh;
		table[1].extra2 = &net->ipv4.frags.high_thresh;
		table[2].data = &net->ipv4.frags.timeout;
		table[3].data = &net->ipv4.frags.max_dist;
	}

	hdr = register_net_sysctl(net, "net/ipv4", table);
	if (!hdr)
		goto err_reg;

	net->ipv4.frags_hdr = hdr;
	return 0;

err_reg:
	if (!net_eq(net, &init_net))
		kfree(table);
err_alloc:
	return -ENOMEM;
}