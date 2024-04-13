int btrfs_bg_type_to_factor(u64 flags)
{
	if (flags & (BTRFS_BLOCK_GROUP_DUP | BTRFS_BLOCK_GROUP_RAID1 |
		     BTRFS_BLOCK_GROUP_RAID10))
		return 2;
	return 1;
}