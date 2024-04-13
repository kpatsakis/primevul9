static void bond_reset_slave_arr(struct bonding *bond)
{
	struct bond_up_slave *usable, *all;

	usable = rtnl_dereference(bond->usable_slaves);
	if (usable) {
		RCU_INIT_POINTER(bond->usable_slaves, NULL);
		kfree_rcu(usable, rcu);
	}

	all = rtnl_dereference(bond->all_slaves);
	if (all) {
		RCU_INIT_POINTER(bond->all_slaves, NULL);
		kfree_rcu(all, rcu);
	}
}