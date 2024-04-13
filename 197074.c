static void bond_set_slave_arr(struct bonding *bond,
			       struct bond_up_slave *usable_slaves,
			       struct bond_up_slave *all_slaves)
{
	struct bond_up_slave *usable, *all;

	usable = rtnl_dereference(bond->usable_slaves);
	rcu_assign_pointer(bond->usable_slaves, usable_slaves);
	kfree_rcu(usable, rcu);

	all = rtnl_dereference(bond->all_slaves);
	rcu_assign_pointer(bond->all_slaves, all_slaves);
	kfree_rcu(all, rcu);
}