static int check_feature_bits(struct btrfs_fs_info *fs_info,
			      enum btrfs_feature_set set,
			      u64 change_mask, u64 flags, u64 supported_flags,
			      u64 safe_set, u64 safe_clear)
{
	const char *type = btrfs_feature_set_names[set];
	char *names;
	u64 disallowed, unsupported;
	u64 set_mask = flags & change_mask;
	u64 clear_mask = ~flags & change_mask;

	unsupported = set_mask & ~supported_flags;
	if (unsupported) {
		names = btrfs_printable_features(set, unsupported);
		if (names) {
			btrfs_warn(fs_info,
				   "this kernel does not support the %s feature bit%s",
				   names, strchr(names, ',') ? "s" : "");
			kfree(names);
		} else
			btrfs_warn(fs_info,
				   "this kernel does not support %s bits 0x%llx",
				   type, unsupported);
		return -EOPNOTSUPP;
	}

	disallowed = set_mask & ~safe_set;
	if (disallowed) {
		names = btrfs_printable_features(set, disallowed);
		if (names) {
			btrfs_warn(fs_info,
				   "can't set the %s feature bit%s while mounted",
				   names, strchr(names, ',') ? "s" : "");
			kfree(names);
		} else
			btrfs_warn(fs_info,
				   "can't set %s bits 0x%llx while mounted",
				   type, disallowed);
		return -EPERM;
	}

	disallowed = clear_mask & ~safe_clear;
	if (disallowed) {
		names = btrfs_printable_features(set, disallowed);
		if (names) {
			btrfs_warn(fs_info,
				   "can't clear the %s feature bit%s while mounted",
				   names, strchr(names, ',') ? "s" : "");
			kfree(names);
		} else
			btrfs_warn(fs_info,
				   "can't clear %s bits 0x%llx while mounted",
				   type, disallowed);
		return -EPERM;
	}

	return 0;
}