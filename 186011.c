int vfs_clone_file_range(struct file *file_in, loff_t pos_in,
		struct file *file_out, loff_t pos_out, u64 len)
{
	struct inode *inode_in = file_inode(file_in);
	struct inode *inode_out = file_inode(file_out);
	int ret;

	if (inode_in->i_sb != inode_out->i_sb ||
	    file_in->f_path.mnt != file_out->f_path.mnt)
		return -EXDEV;

	if (S_ISDIR(inode_in->i_mode) || S_ISDIR(inode_out->i_mode))
		return -EISDIR;
	if (!S_ISREG(inode_in->i_mode) || !S_ISREG(inode_out->i_mode))
		return -EINVAL;

	if (!(file_in->f_mode & FMODE_READ) ||
	    !(file_out->f_mode & FMODE_WRITE) ||
	    (file_out->f_flags & O_APPEND) ||
	    !file_in->f_op->clone_file_range)
		return -EBADF;

	ret = clone_verify_area(file_in, pos_in, len, false);
	if (ret)
		return ret;

	ret = clone_verify_area(file_out, pos_out, len, true);
	if (ret)
		return ret;

	if (pos_in + len > i_size_read(inode_in))
		return -EINVAL;

	ret = mnt_want_write_file(file_out);
	if (ret)
		return ret;

	ret = file_in->f_op->clone_file_range(file_in, pos_in,
			file_out, pos_out, len);
	if (!ret) {
		fsnotify_access(file_in);
		fsnotify_modify(file_out);
	}

	mnt_drop_write_file(file_out);
	return ret;
}