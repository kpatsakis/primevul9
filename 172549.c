bool btrfs_check_rw_degradable(struct btrfs_fs_info *fs_info,
					struct btrfs_device *failing_dev)
{
	struct btrfs_mapping_tree *map_tree = &fs_info->mapping_tree;
	struct extent_map *em;
	u64 next_start = 0;
	bool ret = true;

	read_lock(&map_tree->map_tree.lock);
	em = lookup_extent_mapping(&map_tree->map_tree, 0, (u64)-1);
	read_unlock(&map_tree->map_tree.lock);
	/* No chunk at all? Return false anyway */
	if (!em) {
		ret = false;
		goto out;
	}
	while (em) {
		struct map_lookup *map;
		int missing = 0;
		int max_tolerated;
		int i;

		map = em->map_lookup;
		max_tolerated =
			btrfs_get_num_tolerated_disk_barrier_failures(
					map->type);
		for (i = 0; i < map->num_stripes; i++) {
			struct btrfs_device *dev = map->stripes[i].dev;

			if (!dev || !dev->bdev ||
			    test_bit(BTRFS_DEV_STATE_MISSING, &dev->dev_state) ||
			    dev->last_flush_error)
				missing++;
			else if (failing_dev && failing_dev == dev)
				missing++;
		}
		if (missing > max_tolerated) {
			if (!failing_dev)
				btrfs_warn(fs_info,
	"chunk %llu missing %d devices, max tolerance is %d for writable mount",
				   em->start, missing, max_tolerated);
			free_extent_map(em);
			ret = false;
			goto out;
		}
		next_start = extent_map_end(em);
		free_extent_map(em);

		read_lock(&map_tree->map_tree.lock);
		em = lookup_extent_mapping(&map_tree->map_tree, next_start,
					   (u64)(-1) - next_start);
		read_unlock(&map_tree->map_tree.lock);
	}
out:
	return ret;
}