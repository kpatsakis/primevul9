int __init igmp_mc_init(void)
{
#if defined(CONFIG_PROC_FS)
	int err;

	err = register_pernet_subsys(&igmp_net_ops);
	if (err)
		return err;
	err = register_netdevice_notifier(&igmp_notifier);
	if (err)
		goto reg_notif_fail;
	return 0;

reg_notif_fail:
	unregister_pernet_subsys(&igmp_net_ops);
	return err;
#else
	return register_netdevice_notifier(&igmp_notifier);
#endif
}