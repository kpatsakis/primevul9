static int __init nfnetlink_queue_init(void)
{
	int status = -ENOMEM;

	netlink_register_notifier(&nfqnl_rtnl_notifier);
	status = nfnetlink_subsys_register(&nfqnl_subsys);
	if (status < 0) {
		pr_err("nf_queue: failed to create netlink socket\n");
		goto cleanup_netlink_notifier;
	}

	status = register_pernet_subsys(&nfnl_queue_net_ops);
	if (status < 0) {
		pr_err("nf_queue: failed to register pernet ops\n");
		goto cleanup_subsys;
	}
	register_netdevice_notifier(&nfqnl_dev_notifier);
	nf_register_queue_handler(&nfqh);
	return status;

cleanup_subsys:
	nfnetlink_subsys_unregister(&nfqnl_subsys);
cleanup_netlink_notifier:
	netlink_unregister_notifier(&nfqnl_rtnl_notifier);
	return status;
}