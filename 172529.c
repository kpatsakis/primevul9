static void btrfs_kobject_uevent(struct block_device *bdev,
				 enum kobject_action action)
{
	int ret;

	ret = kobject_uevent(&disk_to_dev(bdev->bd_disk)->kobj, action);
	if (ret)
		pr_warn("BTRFS: Sending event '%d' to kobject: '%s' (%p): failed\n",
			action,
			kobject_name(&disk_to_dev(bdev->bd_disk)->kobj),
			&disk_to_dev(bdev->bd_disk)->kobj);
}