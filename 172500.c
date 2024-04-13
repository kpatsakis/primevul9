int btrfs_ioctl_get_supported_features(void __user *arg)
{
	static const struct btrfs_ioctl_feature_flags features[3] = {
		INIT_FEATURE_FLAGS(SUPP),
		INIT_FEATURE_FLAGS(SAFE_SET),
		INIT_FEATURE_FLAGS(SAFE_CLEAR)
	};

	if (copy_to_user(arg, &features, sizeof(features)))
		return -EFAULT;

	return 0;
}