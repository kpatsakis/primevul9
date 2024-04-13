static int alloc_profile_is_valid(u64 flags, int extended)
{
	u64 mask = (extended ? BTRFS_EXTENDED_PROFILE_MASK :
			       BTRFS_BLOCK_GROUP_PROFILE_MASK);

	flags &= ~BTRFS_BLOCK_GROUP_TYPE_MASK;

	/* 1) check that all other bits are zeroed */
	if (flags & ~mask)
		return 0;

	/* 2) see if profile is reduced */
	if (flags == 0)
		return !extended; /* "0" is valid for usual profiles */

	/* true if exactly one bit set */
	return is_power_of_2(flags);
}