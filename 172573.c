static int contains_pending_extent(struct btrfs_transaction *transaction,
				   struct btrfs_device *device,
				   u64 *start, u64 len)
{
	struct btrfs_fs_info *fs_info = device->fs_info;
	struct extent_map *em;
	struct list_head *search_list = &fs_info->pinned_chunks;
	int ret = 0;
	u64 physical_start = *start;

	if (transaction)
		search_list = &transaction->pending_chunks;
again:
	list_for_each_entry(em, search_list, list) {
		struct map_lookup *map;
		int i;

		map = em->map_lookup;
		for (i = 0; i < map->num_stripes; i++) {
			u64 end;

			if (map->stripes[i].dev != device)
				continue;
			if (map->stripes[i].physical >= physical_start + len ||
			    map->stripes[i].physical + em->orig_block_len <=
			    physical_start)
				continue;
			/*
			 * Make sure that while processing the pinned list we do
			 * not override our *start with a lower value, because
			 * we can have pinned chunks that fall within this
			 * device hole and that have lower physical addresses
			 * than the pending chunks we processed before. If we
			 * do not take this special care we can end up getting
			 * 2 pending chunks that start at the same physical
			 * device offsets because the end offset of a pinned
			 * chunk can be equal to the start offset of some
			 * pending chunk.
			 */
			end = map->stripes[i].physical + em->orig_block_len;
			if (end > *start) {
				*start = end;
				ret = 1;
			}
		}
	}
	if (search_list != &fs_info->pinned_chunks) {
		search_list = &fs_info->pinned_chunks;
		goto again;
	}

	return ret;
}