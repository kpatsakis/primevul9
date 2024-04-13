static int __net_init nfnl_queue_net_init(struct net *net)
{
	unsigned int i;
	struct nfnl_queue_net *q = nfnl_queue_pernet(net);

	for (i = 0; i < INSTANCE_BUCKETS; i++)
		INIT_HLIST_HEAD(&q->instance_table[i]);

	spin_lock_init(&q->instances_lock);

#ifdef CONFIG_PROC_FS
	if (!proc_create("nfnetlink_queue", 0440,
			 net->nf.proc_netfilter, &nfqnl_file_ops))
		return -ENOMEM;
#endif
	return 0;
}