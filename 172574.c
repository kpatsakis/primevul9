const char *get_raid_name(enum btrfs_raid_types type)
{
	if (type >= BTRFS_NR_RAID_TYPES)
		return NULL;

	return btrfs_raid_array[type].raid_name;
}