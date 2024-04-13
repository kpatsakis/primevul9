static void __btrfs_reset_dev_stats(struct btrfs_device *dev)
{
	int i;

	for (i = 0; i < BTRFS_DEV_STAT_VALUES_MAX; i++)
		btrfs_dev_stat_reset(dev, i);
}