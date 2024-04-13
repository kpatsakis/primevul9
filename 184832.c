static void __net_exit nf_ct_frags6_sysctl_unregister(struct net *net)
{
	struct ctl_table *table;

	table = net->nf_frag_frags_hdr->ctl_table_arg;
	unregister_net_sysctl_table(net->nf_frag_frags_hdr);
	if (!net_eq(net, &init_net))
		kfree(table);
}