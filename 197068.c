static void __net_exit bond_net_exit(struct net *net)
{
	struct bond_net *bn = net_generic(net, bond_net_id);
	struct bonding *bond, *tmp_bond;
	LIST_HEAD(list);

	bond_destroy_sysfs(bn);

	/* Kill off any bonds created after unregistering bond rtnl ops */
	rtnl_lock();
	list_for_each_entry_safe(bond, tmp_bond, &bn->dev_list, bond_list)
		unregister_netdevice_queue(bond->dev, &list);
	unregister_netdevice_many(&list);
	rtnl_unlock();

	bond_destroy_proc_dir(bn);
}