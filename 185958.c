static ssize_t compat_do_readv_writev(int type, struct file *file,
			       const struct compat_iovec __user *uvector,
			       unsigned long nr_segs, loff_t *pos)
{
	compat_ssize_t tot_len;
	struct iovec iovstack[UIO_FASTIOV];
	struct iovec *iov = iovstack;
	struct iov_iter iter;
	ssize_t ret;
	io_fn_t fn;
	iter_fn_t iter_fn;

	ret = compat_import_iovec(type, uvector, nr_segs,
				  UIO_FASTIOV, &iov, &iter);
	if (ret < 0)
		return ret;

	tot_len = iov_iter_count(&iter);
	if (!tot_len)
		goto out;
	ret = rw_verify_area(type, file, pos, tot_len);
	if (ret < 0)
		goto out;

	if (type == READ) {
		fn = file->f_op->read;
		iter_fn = file->f_op->read_iter;
	} else {
		fn = (io_fn_t)file->f_op->write;
		iter_fn = file->f_op->write_iter;
		file_start_write(file);
	}

	if (iter_fn)
		ret = do_iter_readv_writev(file, &iter, pos, iter_fn);
	else
		ret = do_loop_readv_writev(file, &iter, pos, fn);

	if (type != READ)
		file_end_write(file);

out:
	kfree(iov);
	if ((ret + (type == READ)) > 0) {
		if (type == READ)
			fsnotify_access(file);
		else
			fsnotify_modify(file);
	}
	return ret;
}