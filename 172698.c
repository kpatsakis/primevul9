static inline int validate_convert_profile(struct btrfs_balance_args *bctl_arg,
		u64 allowed)
{
	return ((bctl_arg->flags & BTRFS_BALANCE_ARGS_CONVERT) &&
		(!alloc_profile_is_valid(bctl_arg->target, 1) ||
		 (bctl_arg->target & ~allowed)));
}