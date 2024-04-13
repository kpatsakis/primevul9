ssize_t vfs_copy_file_range(struct file *file_in, loff_t pos_in,
			    struct file *file_out, loff_t pos_out,
			    size_t len, unsigned int flags)
{
	struct inode *inode_in = file_inode(file_in);
	struct inode *inode_out = file_inode(file_out);
	ssize_t ret;

	if (flags != 0)
		return -EINVAL;

	/* copy_file_range allows full ssize_t len, ignoring MAX_RW_COUNT  */
	ret = rw_verify_area(READ, file_in, &pos_in, len);
	if (ret >= 0)
		ret = rw_verify_area(WRITE, file_out, &pos_out, len);
	if (ret < 0)
		return ret;

	if (!(file_in->f_mode & FMODE_READ) ||
	    !(file_out->f_mode & FMODE_WRITE) ||
	    (file_out->f_flags & O_APPEND))
		return -EBADF;

	/* this could be relaxed once a method supports cross-fs copies */
	if (inode_in->i_sb != inode_out->i_sb)
		return -EXDEV;

	if (len == 0)
		return 0;

	ret = mnt_want_write_file(file_out);
	if (ret)
		return ret;

	ret = -EOPNOTSUPP;
	if (file_out->f_op->copy_file_range)
		ret = file_out->f_op->copy_file_range(file_in, pos_in, file_out,
						      pos_out, len, flags);
	if (ret == -EOPNOTSUPP)
		ret = do_splice_direct(file_in, &pos_in, file_out, &pos_out,
				len > MAX_RW_COUNT ? MAX_RW_COUNT : len, 0);

	if (ret > 0) {
		fsnotify_access(file_in);
		add_rchar(current, ret);
		fsnotify_modify(file_out);
		add_wchar(current, ret);
	}
	inc_syscr(current);
	inc_syscw(current);

	mnt_drop_write_file(file_out);

	return ret;
}