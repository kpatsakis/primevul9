int btrfs_rmap_block(struct btrfs_fs_info *fs_info, u64 chunk_start,
		     u64 physical, u64 **logical, int *naddrs, int *stripe_len)
{
	struct extent_map *em;
	struct map_lookup *map;
	u64 *buf;
	u64 bytenr;
	u64 length;
	u64 stripe_nr;
	u64 rmap_len;
	int i, j, nr = 0;

	em = btrfs_get_chunk_map(fs_info, chunk_start, 1);
	if (IS_ERR(em))
		return -EIO;

	map = em->map_lookup;
	length = em->len;
	rmap_len = map->stripe_len;

	if (map->type & BTRFS_BLOCK_GROUP_RAID10)
		length = div_u64(length, map->num_stripes / map->sub_stripes);
	else if (map->type & BTRFS_BLOCK_GROUP_RAID0)
		length = div_u64(length, map->num_stripes);
	else if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) {
		length = div_u64(length, nr_data_stripes(map));
		rmap_len = map->stripe_len * nr_data_stripes(map);
	}

	buf = kcalloc(map->num_stripes, sizeof(u64), GFP_NOFS);
	BUG_ON(!buf); /* -ENOMEM */

	for (i = 0; i < map->num_stripes; i++) {
		if (map->stripes[i].physical > physical ||
		    map->stripes[i].physical + length <= physical)
			continue;

		stripe_nr = physical - map->stripes[i].physical;
		stripe_nr = div64_u64(stripe_nr, map->stripe_len);

		if (map->type & BTRFS_BLOCK_GROUP_RAID10) {
			stripe_nr = stripe_nr * map->num_stripes + i;
			stripe_nr = div_u64(stripe_nr, map->sub_stripes);
		} else if (map->type & BTRFS_BLOCK_GROUP_RAID0) {
			stripe_nr = stripe_nr * map->num_stripes + i;
		} /* else if RAID[56], multiply by nr_data_stripes().
		   * Alternatively, just use rmap_len below instead of
		   * map->stripe_len */

		bytenr = chunk_start + stripe_nr * rmap_len;
		WARN_ON(nr >= map->num_stripes);
		for (j = 0; j < nr; j++) {
			if (buf[j] == bytenr)
				break;
		}
		if (j == nr) {
			WARN_ON(nr >= map->num_stripes);
			buf[nr++] = bytenr;
		}
	}

	*logical = buf;
	*naddrs = nr;
	*stripe_len = rmap_len;

	free_extent_map(em);
	return 0;
}