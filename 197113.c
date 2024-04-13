static void bond_slave_arr_handler(struct work_struct *work)
{
	struct bonding *bond = container_of(work, struct bonding,
					    slave_arr_work.work);
	int ret;

	if (!rtnl_trylock())
		goto err;

	ret = bond_update_slave_arr(bond, NULL);
	rtnl_unlock();
	if (ret) {
		pr_warn_ratelimited("Failed to update slave array from WT\n");
		goto err;
	}
	return;

err:
	bond_slave_arr_work_rearm(bond, 1);
}