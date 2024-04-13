static char* btrfs_dev_name(struct btrfs_device *device)
{
	if (!device || test_bit(BTRFS_DEV_STATE_MISSING, &device->dev_state))
		return "<missing disk>";
	else
		return rcu_str_deref(device->name);
}