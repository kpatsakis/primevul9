static void slave_kobj_release(struct kobject *kobj)
{
	struct slave *slave = to_slave(kobj);
	struct bonding *bond = bond_get_bond_by_slave(slave);

	cancel_delayed_work_sync(&slave->notify_work);
	if (BOND_MODE(bond) == BOND_MODE_8023AD)
		kfree(SLAVE_AD_INFO(slave));

	kfree(slave);
}