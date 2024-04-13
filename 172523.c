int btrfs_run_dev_stats(struct btrfs_trans_handle *trans,
			struct btrfs_fs_info *fs_info)
{
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	struct btrfs_device *device;
	int stats_cnt;
	int ret = 0;

	mutex_lock(&fs_devices->device_list_mutex);
	list_for_each_entry(device, &fs_devices->devices, dev_list) {
		stats_cnt = atomic_read(&device->dev_stats_ccnt);
		if (!device->dev_stats_valid || stats_cnt == 0)
			continue;


		/*
		 * There is a LOAD-LOAD control dependency between the value of
		 * dev_stats_ccnt and updating the on-disk values which requires
		 * reading the in-memory counters. Such control dependencies
		 * require explicit read memory barriers.
		 *
		 * This memory barriers pairs with smp_mb__before_atomic in
		 * btrfs_dev_stat_inc/btrfs_dev_stat_set and with the full
		 * barrier implied by atomic_xchg in
		 * btrfs_dev_stats_read_and_reset
		 */
		smp_rmb();

		ret = update_dev_stat_item(trans, device);
		if (!ret)
			atomic_sub(stats_cnt, &device->dev_stats_ccnt);
	}
	mutex_unlock(&fs_devices->device_list_mutex);

	return ret;
}