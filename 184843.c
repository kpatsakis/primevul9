static int nf_ct_frag6_sysctl_register(struct net *net)
{
	struct ctl_table *table;
	struct ctl_table_header *hdr;

	table = nf_ct_frag6_sysctl_table;
	if (!net_eq(net, &init_net)) {
		table = kmemdup(table, sizeof(nf_ct_frag6_sysctl_table),
				GFP_KERNEL);
		if (table == NULL)
			goto err_alloc;

		table[0].data = &net->nf_frag.frags.timeout;
		table[1].data = &net->nf_frag.frags.low_thresh;
		table[1].extra2 = &net->nf_frag.frags.high_thresh;
		table[2].data = &net->nf_frag.frags.high_thresh;
		table[2].extra1 = &net->nf_frag.frags.low_thresh;
		table[2].extra2 = &init_net.nf_frag.frags.high_thresh;
	}

	hdr = register_net_sysctl(net, "net/netfilter", table);
	if (hdr == NULL)
		goto err_reg;

	net->nf_frag_frags_hdr = hdr;
	return 0;

err_reg:
	if (!net_eq(net, &init_net))
		kfree(table);
err_alloc:
	return -ENOMEM;
}