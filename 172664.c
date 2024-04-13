static void btrfs_dev_replace_update_device_in_mapping_tree(
						struct btrfs_fs_info *fs_info,
						struct btrfs_device *srcdev,
						struct btrfs_device *tgtdev)
{
	struct extent_map_tree *em_tree = &fs_info->mapping_tree.map_tree;
	struct extent_map *em;
	struct map_lookup *map;
	u64 start = 0;
	int i;

	write_lock(&em_tree->lock);
	do {
		em = lookup_extent_mapping(em_tree, start, (u64)-1);
		if (!em)
			break;
		map = em->map_lookup;
		for (i = 0; i < map->num_stripes; i++)
			if (srcdev == map->stripes[i].dev)
				map->stripes[i].dev = tgtdev;
		start = em->start + em->len;
		free_extent_map(em);
	} while (start);
	write_unlock(&em_tree->lock);
}