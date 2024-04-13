int btrfs_num_copies(struct btrfs_fs_info *fs_info, u64 logical, u64 len)
{
	struct extent_map *em;
	struct map_lookup *map;
	int ret;

	em = btrfs_get_chunk_map(fs_info, logical, len);
	if (IS_ERR(em))
		/*
		 * We could return errors for these cases, but that could get
		 * ugly and we'd probably do the same thing which is just not do
		 * anything else and exit, so return 1 so the callers don't try
		 * to use other copies.
		 */
		return 1;

	map = em->map_lookup;
	if (map->type & (BTRFS_BLOCK_GROUP_DUP | BTRFS_BLOCK_GROUP_RAID1))
		ret = map->num_stripes;
	else if (map->type & BTRFS_BLOCK_GROUP_RAID10)
		ret = map->sub_stripes;
	else if (map->type & BTRFS_BLOCK_GROUP_RAID5)
		ret = 2;
	else if (map->type & BTRFS_BLOCK_GROUP_RAID6)
		/*
		 * There could be two corrupted data stripes, we need
		 * to loop retry in order to rebuild the correct data.
		 *
		 * Fail a stripe at a time on every retry except the
		 * stripe under reconstruction.
		 */
		ret = map->num_stripes;
	else
		ret = 1;
	free_extent_map(em);

	down_read(&fs_info->dev_replace.rwsem);
	if (btrfs_dev_replace_is_ongoing(&fs_info->dev_replace) &&
	    fs_info->dev_replace.tgtdev)
		ret++;
	up_read(&fs_info->dev_replace.rwsem);

	return ret;
}