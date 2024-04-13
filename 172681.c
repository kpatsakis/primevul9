static void free_device_rcu(struct rcu_head *head)
{
	struct btrfs_device *device;

	device = container_of(head, struct btrfs_device, rcu);
	btrfs_free_device(device);
}