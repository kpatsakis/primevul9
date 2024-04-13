int ext4_resize_begin(struct super_block *sb)
{
	int ret = 0;

	if (!capable(CAP_SYS_RESOURCE))
		return -EPERM;

	/*
	 * We are not allowed to do online-resizing on a filesystem mounted
	 * with error, because it can destroy the filesystem easily.
	 */
	if (EXT4_SB(sb)->s_mount_state & EXT4_ERROR_FS) {
		ext4_warning(sb, "There are errors in the filesystem, "
			     "so online resizing is not allowed\n");
		return -EPERM;
	}

	if (test_and_set_bit_lock(EXT4_RESIZING, &EXT4_SB(sb)->s_resize_flags))
		ret = -EBUSY;

	return ret;
}