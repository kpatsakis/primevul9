int btrfs_init_dev_stats(struct btrfs_fs_info *fs_info)
{
	struct btrfs_key key;
	struct btrfs_key found_key;
	struct btrfs_root *dev_root = fs_info->dev_root;
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	struct extent_buffer *eb;
	int slot;
	int ret = 0;
	struct btrfs_device *device;
	struct btrfs_path *path = NULL;
	int i;

	path = btrfs_alloc_path();
	if (!path) {
		ret = -ENOMEM;
		goto out;
	}

	mutex_lock(&fs_devices->device_list_mutex);
	list_for_each_entry(device, &fs_devices->devices, dev_list) {
		int item_size;
		struct btrfs_dev_stats_item *ptr;

		key.objectid = BTRFS_DEV_STATS_OBJECTID;
		key.type = BTRFS_PERSISTENT_ITEM_KEY;
		key.offset = device->devid;
		ret = btrfs_search_slot(NULL, dev_root, &key, path, 0, 0);
		if (ret) {
			__btrfs_reset_dev_stats(device);
			device->dev_stats_valid = 1;
			btrfs_release_path(path);
			continue;
		}
		slot = path->slots[0];
		eb = path->nodes[0];
		btrfs_item_key_to_cpu(eb, &found_key, slot);
		item_size = btrfs_item_size_nr(eb, slot);

		ptr = btrfs_item_ptr(eb, slot,
				     struct btrfs_dev_stats_item);

		for (i = 0; i < BTRFS_DEV_STAT_VALUES_MAX; i++) {
			if (item_size >= (1 + i) * sizeof(__le64))
				btrfs_dev_stat_set(device, i,
					btrfs_dev_stats_value(eb, ptr, i));
			else
				btrfs_dev_stat_reset(device, i);
		}

		device->dev_stats_valid = 1;
		btrfs_dev_stat_print_on_load(device);
		btrfs_release_path(path);
	}
	mutex_unlock(&fs_devices->device_list_mutex);

out:
	btrfs_free_path(path);
	return ret < 0 ? ret : 0;
}