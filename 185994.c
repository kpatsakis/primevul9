static int ioctl_fsthaw(struct file *filp)
{
	struct super_block *sb = file_inode(filp)->i_sb;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	/* Thaw */
	if (sb->s_op->thaw_super)
		return sb->s_op->thaw_super(sb);
	return thaw_super(sb);
}