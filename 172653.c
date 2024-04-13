int btrfs_dev_replace_by_ioctl(struct btrfs_fs_info *fs_info,
			    struct btrfs_ioctl_dev_replace_args *args)
{
	int ret;

	switch (args->start.cont_reading_from_srcdev_mode) {
	case BTRFS_IOCTL_DEV_REPLACE_CONT_READING_FROM_SRCDEV_MODE_ALWAYS:
	case BTRFS_IOCTL_DEV_REPLACE_CONT_READING_FROM_SRCDEV_MODE_AVOID:
		break;
	default:
		return -EINVAL;
	}

	if ((args->start.srcdevid == 0 && args->start.srcdev_name[0] == '\0') ||
	    args->start.tgtdev_name[0] == '\0')
		return -EINVAL;

	ret = btrfs_dev_replace_start(fs_info, args->start.tgtdev_name,
					args->start.srcdevid,
					args->start.srcdev_name,
					args->start.cont_reading_from_srcdev_mode);
	args->result = ret;
	/* don't warn if EINPROGRESS, someone else might be running scrub */
	if (ret == BTRFS_IOCTL_DEV_REPLACE_RESULT_SCRUB_INPROGRESS ||
	    ret == BTRFS_IOCTL_DEV_REPLACE_RESULT_NO_ERROR)
		return 0;

	return ret;
}