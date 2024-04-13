static struct btrfs_fs_devices *alloc_fs_devices(const u8 *fsid,
						 const u8 *metadata_fsid)
{
	struct btrfs_fs_devices *fs_devs;

	fs_devs = kzalloc(sizeof(*fs_devs), GFP_KERNEL);
	if (!fs_devs)
		return ERR_PTR(-ENOMEM);

	mutex_init(&fs_devs->device_list_mutex);

	INIT_LIST_HEAD(&fs_devs->devices);
	INIT_LIST_HEAD(&fs_devs->resized_devices);
	INIT_LIST_HEAD(&fs_devs->alloc_list);
	INIT_LIST_HEAD(&fs_devs->fs_list);
	if (fsid)
		memcpy(fs_devs->fsid, fsid, BTRFS_FSID_SIZE);

	if (metadata_fsid)
		memcpy(fs_devs->metadata_uuid, metadata_fsid, BTRFS_FSID_SIZE);
	else if (fsid)
		memcpy(fs_devs->metadata_uuid, fsid, BTRFS_FSID_SIZE);

	return fs_devs;
}