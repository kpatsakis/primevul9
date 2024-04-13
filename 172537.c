static int btrfs_prepare_sprout(struct btrfs_fs_info *fs_info)
{
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	struct btrfs_fs_devices *old_devices;
	struct btrfs_fs_devices *seed_devices;
	struct btrfs_super_block *disk_super = fs_info->super_copy;
	struct btrfs_device *device;
	u64 super_flags;

	lockdep_assert_held(&uuid_mutex);
	if (!fs_devices->seeding)
		return -EINVAL;

	seed_devices = alloc_fs_devices(NULL, NULL);
	if (IS_ERR(seed_devices))
		return PTR_ERR(seed_devices);

	old_devices = clone_fs_devices(fs_devices);
	if (IS_ERR(old_devices)) {
		kfree(seed_devices);
		return PTR_ERR(old_devices);
	}

	list_add(&old_devices->fs_list, &fs_uuids);

	memcpy(seed_devices, fs_devices, sizeof(*seed_devices));
	seed_devices->opened = 1;
	INIT_LIST_HEAD(&seed_devices->devices);
	INIT_LIST_HEAD(&seed_devices->alloc_list);
	mutex_init(&seed_devices->device_list_mutex);

	mutex_lock(&fs_devices->device_list_mutex);
	list_splice_init_rcu(&fs_devices->devices, &seed_devices->devices,
			      synchronize_rcu);
	list_for_each_entry(device, &seed_devices->devices, dev_list)
		device->fs_devices = seed_devices;

	mutex_lock(&fs_info->chunk_mutex);
	list_splice_init(&fs_devices->alloc_list, &seed_devices->alloc_list);
	mutex_unlock(&fs_info->chunk_mutex);

	fs_devices->seeding = 0;
	fs_devices->num_devices = 0;
	fs_devices->open_devices = 0;
	fs_devices->missing_devices = 0;
	fs_devices->rotating = 0;
	fs_devices->seed = seed_devices;

	generate_random_uuid(fs_devices->fsid);
	memcpy(fs_devices->metadata_uuid, fs_devices->fsid, BTRFS_FSID_SIZE);
	memcpy(disk_super->fsid, fs_devices->fsid, BTRFS_FSID_SIZE);
	mutex_unlock(&fs_devices->device_list_mutex);

	super_flags = btrfs_super_flags(disk_super) &
		      ~BTRFS_SUPER_FLAG_SEEDING;
	btrfs_set_super_flags(disk_super, super_flags);

	return 0;
}