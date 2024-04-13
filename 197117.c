static int __init bonding_init(void)
{
	int i;
	int res;

	res = bond_check_params(&bonding_defaults);
	if (res)
		goto out;

	res = register_pernet_subsys(&bond_net_ops);
	if (res)
		goto out;

	res = bond_netlink_init();
	if (res)
		goto err_link;

	bond_create_debugfs();

	for (i = 0; i < max_bonds; i++) {
		res = bond_create(&init_net, NULL);
		if (res)
			goto err;
	}

	skb_flow_dissector_init(&flow_keys_bonding,
				flow_keys_bonding_keys,
				ARRAY_SIZE(flow_keys_bonding_keys));

	register_netdevice_notifier(&bond_netdev_notifier);
out:
	return res;
err:
	bond_destroy_debugfs();
	bond_netlink_fini();
err_link:
	unregister_pernet_subsys(&bond_net_ops);
	goto out;

}