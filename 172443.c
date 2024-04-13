int btrfs_finish_chunk_alloc(struct btrfs_trans_handle *trans,
			     u64 chunk_offset, u64 chunk_size)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	struct btrfs_root *extent_root = fs_info->extent_root;
	struct btrfs_root *chunk_root = fs_info->chunk_root;
	struct btrfs_key key;
	struct btrfs_device *device;
	struct btrfs_chunk *chunk;
	struct btrfs_stripe *stripe;
	struct extent_map *em;
	struct map_lookup *map;
	size_t item_size;
	u64 dev_offset;
	u64 stripe_size;
	int i = 0;
	int ret = 0;

	em = btrfs_get_chunk_map(fs_info, chunk_offset, chunk_size);
	if (IS_ERR(em))
		return PTR_ERR(em);

	map = em->map_lookup;
	item_size = btrfs_chunk_item_size(map->num_stripes);
	stripe_size = em->orig_block_len;

	chunk = kzalloc(item_size, GFP_NOFS);
	if (!chunk) {
		ret = -ENOMEM;
		goto out;
	}

	/*
	 * Take the device list mutex to prevent races with the final phase of
	 * a device replace operation that replaces the device object associated
	 * with the map's stripes, because the device object's id can change
	 * at any time during that final phase of the device replace operation
	 * (dev-replace.c:btrfs_dev_replace_finishing()).
	 */
	mutex_lock(&fs_info->fs_devices->device_list_mutex);
	for (i = 0; i < map->num_stripes; i++) {
		device = map->stripes[i].dev;
		dev_offset = map->stripes[i].physical;

		ret = btrfs_update_device(trans, device);
		if (ret)
			break;
		ret = btrfs_alloc_dev_extent(trans, device, chunk_offset,
					     dev_offset, stripe_size);
		if (ret)
			break;
	}
	if (ret) {
		mutex_unlock(&fs_info->fs_devices->device_list_mutex);
		goto out;
	}

	stripe = &chunk->stripe;
	for (i = 0; i < map->num_stripes; i++) {
		device = map->stripes[i].dev;
		dev_offset = map->stripes[i].physical;

		btrfs_set_stack_stripe_devid(stripe, device->devid);
		btrfs_set_stack_stripe_offset(stripe, dev_offset);
		memcpy(stripe->dev_uuid, device->uuid, BTRFS_UUID_SIZE);
		stripe++;
	}
	mutex_unlock(&fs_info->fs_devices->device_list_mutex);

	btrfs_set_stack_chunk_length(chunk, chunk_size);
	btrfs_set_stack_chunk_owner(chunk, extent_root->root_key.objectid);
	btrfs_set_stack_chunk_stripe_len(chunk, map->stripe_len);
	btrfs_set_stack_chunk_type(chunk, map->type);
	btrfs_set_stack_chunk_num_stripes(chunk, map->num_stripes);
	btrfs_set_stack_chunk_io_align(chunk, map->stripe_len);
	btrfs_set_stack_chunk_io_width(chunk, map->stripe_len);
	btrfs_set_stack_chunk_sector_size(chunk, fs_info->sectorsize);
	btrfs_set_stack_chunk_sub_stripes(chunk, map->sub_stripes);

	key.objectid = BTRFS_FIRST_CHUNK_TREE_OBJECTID;
	key.type = BTRFS_CHUNK_ITEM_KEY;
	key.offset = chunk_offset;

	ret = btrfs_insert_item(trans, chunk_root, &key, chunk, item_size);
	if (ret == 0 && map->type & BTRFS_BLOCK_GROUP_SYSTEM) {
		/*
		 * TODO: Cleanup of inserted chunk root in case of
		 * failure.
		 */
		ret = btrfs_add_system_chunk(fs_info, &key, chunk, item_size);
	}

out:
	kfree(chunk);
	free_extent_map(em);
	return ret;
}