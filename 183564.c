static int __net_init igmp_net_init(struct net *net)
{
	struct proc_dir_entry *pde;
	int err;

	pde = proc_create_net("igmp", 0444, net->proc_net, &igmp_mc_seq_ops,
			sizeof(struct igmp_mc_iter_state));
	if (!pde)
		goto out_igmp;
	pde = proc_create_net("mcfilter", 0444, net->proc_net,
			&igmp_mcf_seq_ops, sizeof(struct igmp_mcf_iter_state));
	if (!pde)
		goto out_mcfilter;
	err = inet_ctl_sock_create(&net->ipv4.mc_autojoin_sk, AF_INET,
				   SOCK_DGRAM, 0, net);
	if (err < 0) {
		pr_err("Failed to initialize the IGMP autojoin socket (err %d)\n",
		       err);
		goto out_sock;
	}

	return 0;

out_sock:
	remove_proc_entry("mcfilter", net->proc_net);
out_mcfilter:
	remove_proc_entry("igmp", net->proc_net);
out_igmp:
	return -ENOMEM;
}