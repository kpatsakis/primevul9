void btrfs_update_commit_device_size(struct btrfs_fs_info *fs_info)
{
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	struct btrfs_device *curr, *next;

	if (list_empty(&fs_devices->resized_devices))
		return;

	mutex_lock(&fs_devices->device_list_mutex);
	mutex_lock(&fs_info->chunk_mutex);
	list_for_each_entry_safe(curr, next, &fs_devices->resized_devices,
				 resized_list) {
		list_del_init(&curr->resized_list);
		curr->commit_total_bytes = curr->disk_total_bytes;
	}
	mutex_unlock(&fs_info->chunk_mutex);
	mutex_unlock(&fs_devices->device_list_mutex);
}