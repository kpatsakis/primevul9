static noinline_for_stack int scrub_chunk(struct scrub_ctx *sctx,
					  struct btrfs_device *scrub_dev,
					  u64 chunk_offset, u64 length,
					  u64 dev_offset,
					  struct btrfs_block_group_cache *cache)
{
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	struct btrfs_mapping_tree *map_tree = &fs_info->mapping_tree;
	struct map_lookup *map;
	struct extent_map *em;
	int i;
	int ret = 0;

	read_lock(&map_tree->map_tree.lock);
	em = lookup_extent_mapping(&map_tree->map_tree, chunk_offset, 1);
	read_unlock(&map_tree->map_tree.lock);

	if (!em) {
		/*
		 * Might have been an unused block group deleted by the cleaner
		 * kthread or relocation.
		 */
		spin_lock(&cache->lock);
		if (!cache->removed)
			ret = -EINVAL;
		spin_unlock(&cache->lock);

		return ret;
	}

	map = em->map_lookup;
	if (em->start != chunk_offset)
		goto out;

	if (em->len < length)
		goto out;

	for (i = 0; i < map->num_stripes; ++i) {
		if (map->stripes[i].dev->bdev == scrub_dev->bdev &&
		    map->stripes[i].physical == dev_offset) {
			ret = scrub_stripe(sctx, map, scrub_dev, i,
					   chunk_offset, length);
			if (ret)
				goto out;
		}
	}
out:
	free_extent_map(em);

	return ret;
}