static u64 calc_stripe_length(u64 type, u64 chunk_len, int num_stripes)
{
	int index = btrfs_bg_flags_to_raid_index(type);
	int ncopies = btrfs_raid_array[index].ncopies;
	int data_stripes;

	switch (type & BTRFS_BLOCK_GROUP_PROFILE_MASK) {
	case BTRFS_BLOCK_GROUP_RAID5:
		data_stripes = num_stripes - 1;
		break;
	case BTRFS_BLOCK_GROUP_RAID6:
		data_stripes = num_stripes - 2;
		break;
	default:
		data_stripes = num_stripes / ncopies;
		break;
	}
	return div_u64(chunk_len, data_stripes);
}