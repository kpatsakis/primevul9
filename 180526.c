static bool device_has_rmrr(struct device *dev)
{
	struct dmar_rmrr_unit *rmrr;
	struct device *tmp;
	int i;

	rcu_read_lock();
	for_each_rmrr_units(rmrr) {
		/*
		 * Return TRUE if this RMRR contains the device that
		 * is passed in.
		 */
		for_each_active_dev_scope(rmrr->devices,
					  rmrr->devices_cnt, i, tmp)
			if (tmp == dev) {
				rcu_read_unlock();
				return true;
			}
	}
	rcu_read_unlock();
	return false;
}