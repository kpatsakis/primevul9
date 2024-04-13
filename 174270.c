static int selinux_is_sblabel_mnt(struct super_block *sb)
{
	struct superblock_security_struct *sbsec = sb->s_security;

	/*
	 * IMPORTANT: Double-check logic in this function when adding a new
	 * SECURITY_FS_USE_* definition!
	 */
	BUILD_BUG_ON(SECURITY_FS_USE_MAX != 7);

	switch (sbsec->behavior) {
	case SECURITY_FS_USE_XATTR:
	case SECURITY_FS_USE_TRANS:
	case SECURITY_FS_USE_TASK:
	case SECURITY_FS_USE_NATIVE:
		return 1;

	case SECURITY_FS_USE_GENFS:
		return selinux_is_genfs_special_handling(sb);

	/* Never allow relabeling on context mounts */
	case SECURITY_FS_USE_MNTPOINT:
	case SECURITY_FS_USE_NONE:
	default:
		return 0;
	}
}