static inline int btrfs_chunk_max_errors(struct map_lookup *map)
{
	int max_errors;

	if (map->type & (BTRFS_BLOCK_GROUP_RAID1 |
			 BTRFS_BLOCK_GROUP_RAID10 |
			 BTRFS_BLOCK_GROUP_RAID5 |
			 BTRFS_BLOCK_GROUP_DUP)) {
		max_errors = 1;
	} else if (map->type & BTRFS_BLOCK_GROUP_RAID6) {
		max_errors = 2;
	} else {
		max_errors = 0;
	}

	return max_errors;
}