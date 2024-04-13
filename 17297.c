int do_fallocate(struct file *file, int mode, loff_t offset, loff_t len)
{
	struct inode *inode = file_inode(file);
	long ret;

	if (offset < 0 || len <= 0)
		return -EINVAL;

	/* Return error if mode is not supported */
	if (mode & ~(FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE))
		return -EOPNOTSUPP;

	/* Punch hole must have keep size set */
	if ((mode & FALLOC_FL_PUNCH_HOLE) &&
	    !(mode & FALLOC_FL_KEEP_SIZE))
		return -EOPNOTSUPP;

	if (!(file->f_mode & FMODE_WRITE))
		return -EBADF;

	/* It's not possible punch hole on append only file */
	if (mode & FALLOC_FL_PUNCH_HOLE && IS_APPEND(inode))
		return -EPERM;

	if (IS_IMMUTABLE(inode))
		return -EPERM;

	/*
	 * Revalidate the write permissions, in case security policy has
	 * changed since the files were opened.
	 */
	ret = security_file_permission(file, MAY_WRITE);
	if (ret)
		return ret;

	if (S_ISFIFO(inode->i_mode))
		return -ESPIPE;

	/*
	 * Let individual file system decide if it supports preallocation
	 * for directories or not.
	 */
	if (!S_ISREG(inode->i_mode) && !S_ISDIR(inode->i_mode))
		return -ENODEV;

	/* Check for wrap through zero too */
	if (((offset + len) > inode->i_sb->s_maxbytes) || ((offset + len) < 0))
		return -EFBIG;

	if (!file->f_op->fallocate)
		return -EOPNOTSUPP;

	sb_start_write(inode->i_sb);
	ret = file->f_op->fallocate(file, mode, offset, len);
	sb_end_write(inode->i_sb);
	return ret;
}