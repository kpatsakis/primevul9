int btrfs_remove_chunk(struct btrfs_trans_handle *trans, u64 chunk_offset)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	struct extent_map *em;
	struct map_lookup *map;
	u64 dev_extent_len = 0;
	int i, ret = 0;
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;

	em = btrfs_get_chunk_map(fs_info, chunk_offset, 1);
	if (IS_ERR(em)) {
		/*
		 * This is a logic error, but we don't want to just rely on the
		 * user having built with ASSERT enabled, so if ASSERT doesn't
		 * do anything we still error out.
		 */
		ASSERT(0);
		return PTR_ERR(em);
	}
	map = em->map_lookup;
	mutex_lock(&fs_info->chunk_mutex);
	check_system_chunk(trans, map->type);
	mutex_unlock(&fs_info->chunk_mutex);

	/*
	 * Take the device list mutex to prevent races with the final phase of
	 * a device replace operation that replaces the device object associated
	 * with map stripes (dev-replace.c:btrfs_dev_replace_finishing()).
	 */
	mutex_lock(&fs_devices->device_list_mutex);
	for (i = 0; i < map->num_stripes; i++) {
		struct btrfs_device *device = map->stripes[i].dev;
		ret = btrfs_free_dev_extent(trans, device,
					    map->stripes[i].physical,
					    &dev_extent_len);
		if (ret) {
			mutex_unlock(&fs_devices->device_list_mutex);
			btrfs_abort_transaction(trans, ret);
			goto out;
		}

		if (device->bytes_used > 0) {
			mutex_lock(&fs_info->chunk_mutex);
			btrfs_device_set_bytes_used(device,
					device->bytes_used - dev_extent_len);
			atomic64_add(dev_extent_len, &fs_info->free_chunk_space);
			btrfs_clear_space_info_full(fs_info);
			mutex_unlock(&fs_info->chunk_mutex);
		}

		ret = btrfs_update_device(trans, device);
		if (ret) {
			mutex_unlock(&fs_devices->device_list_mutex);
			btrfs_abort_transaction(trans, ret);
			goto out;
		}
	}
	mutex_unlock(&fs_devices->device_list_mutex);

	ret = btrfs_free_chunk(trans, chunk_offset);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		goto out;
	}

	trace_btrfs_chunk_free(fs_info, map, chunk_offset, em->len);

	if (map->type & BTRFS_BLOCK_GROUP_SYSTEM) {
		ret = btrfs_del_sys_chunk(fs_info, chunk_offset);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}
	}

	ret = btrfs_remove_block_group(trans, chunk_offset, em);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		goto out;
	}

out:
	/* once for us */
	free_extent_map(em);
	return ret;
}