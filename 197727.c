static __init int cgw_module_init(void)
{
	int ret;

	/* sanitize given module parameter */
	max_hops = clamp_t(unsigned int, max_hops, CGW_MIN_HOPS, CGW_MAX_HOPS);

	pr_info("can: netlink gateway (rev " CAN_GW_VERSION ") max_hops=%d\n",
		max_hops);

	register_pernet_subsys(&cangw_pernet_ops);
	cgw_cache = kmem_cache_create("can_gw", sizeof(struct cgw_job),
				      0, 0, NULL);

	if (!cgw_cache)
		return -ENOMEM;

	/* set notifier */
	notifier.notifier_call = cgw_notifier;
	register_netdevice_notifier(&notifier);

	ret = rtnl_register_module(THIS_MODULE, PF_CAN, RTM_GETROUTE,
				   NULL, cgw_dump_jobs, 0);
	if (ret) {
		unregister_netdevice_notifier(&notifier);
		kmem_cache_destroy(cgw_cache);
		return -ENOBUFS;
	}

	/* Only the first call to rtnl_register_module can fail */
	rtnl_register_module(THIS_MODULE, PF_CAN, RTM_NEWROUTE,
			     cgw_create_job, NULL, 0);
	rtnl_register_module(THIS_MODULE, PF_CAN, RTM_DELROUTE,
			     cgw_remove_job, NULL, 0);

	return 0;
}