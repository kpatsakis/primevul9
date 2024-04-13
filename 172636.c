void btrfs_free_device(struct btrfs_device *device)
{
	rcu_string_free(device->name);
	bio_put(device->flush_bio);
	kfree(device);
}