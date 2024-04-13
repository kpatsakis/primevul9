static inline int scrub_nr_raid_mirrors(struct btrfs_bio *bbio)
{
	if (bbio->map_type & BTRFS_BLOCK_GROUP_RAID5)
		return 2;
	else if (bbio->map_type & BTRFS_BLOCK_GROUP_RAID6)
		return 3;
	else
		return (int)bbio->num_stripes;
}