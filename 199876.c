static ssize_t compat_do_readv_writev(int type, struct file *file,
			       const struct compat_iovec __user *uvector,
			       unsigned long nr_segs, loff_t *pos)
{
	compat_ssize_t tot_len;
	struct iovec iovstack[UIO_FASTIOV];
	struct iovec *iov=iovstack, *vector;
	ssize_t ret;
	int seg;
	io_fn_t fn;
	iov_fn_t fnv;

	/*
	 * SuS says "The readv() function *may* fail if the iovcnt argument
	 * was less than or equal to 0, or greater than {IOV_MAX}.  Linux has
	 * traditionally returned zero for zero segments, so...
	 */
	ret = 0;
	if (nr_segs == 0)
		goto out;

	/*
	 * First get the "struct iovec" from user memory and
	 * verify all the pointers
	 */
	ret = -EINVAL;
	if ((nr_segs > UIO_MAXIOV) || (nr_segs <= 0))
		goto out;
	if (!file->f_op)
		goto out;
	if (nr_segs > UIO_FASTIOV) {
		ret = -ENOMEM;
		iov = kmalloc(nr_segs*sizeof(struct iovec), GFP_KERNEL);
		if (!iov)
			goto out;
	}
	ret = -EFAULT;
	if (!access_ok(VERIFY_READ, uvector, nr_segs*sizeof(*uvector)))
		goto out;

	/*
	 * Single unix specification:
	 * We should -EINVAL if an element length is not >= 0 and fitting an
	 * ssize_t.  The total length is fitting an ssize_t
	 *
	 * Be careful here because iov_len is a size_t not an ssize_t
	 */
	tot_len = 0;
	vector = iov;
	ret = -EINVAL;
	for (seg = 0 ; seg < nr_segs; seg++) {
		compat_ssize_t tmp = tot_len;
		compat_ssize_t len;
		compat_uptr_t buf;

		if (__get_user(len, &uvector->iov_len) ||
		    __get_user(buf, &uvector->iov_base)) {
			ret = -EFAULT;
			goto out;
		}
		if (len < 0)	/* size_t not fitting an compat_ssize_t .. */
			goto out;
		tot_len += len;
		if (tot_len < tmp) /* maths overflow on the compat_ssize_t */
			goto out;
		vector->iov_base = compat_ptr(buf);
		vector->iov_len = (compat_size_t) len;
		uvector++;
		vector++;
	}
	if (tot_len == 0) {
		ret = 0;
		goto out;
	}

	ret = rw_verify_area(type, file, pos, tot_len);
	if (ret < 0)
		goto out;

	ret = security_file_permission(file, type == READ ? MAY_READ:MAY_WRITE);
	if (ret)
		goto out;

	fnv = NULL;
	if (type == READ) {
		fn = file->f_op->read;
		fnv = file->f_op->aio_read;
	} else {
		fn = (io_fn_t)file->f_op->write;
		fnv = file->f_op->aio_write;
	}

	if (fnv)
		ret = do_sync_readv_writev(file, iov, nr_segs, tot_len,
						pos, fnv);
	else
		ret = do_loop_readv_writev(file, iov, nr_segs, pos, fn);

out:
	if (iov != iovstack)
		kfree(iov);
	if ((ret + (type == READ)) > 0) {
		struct dentry *dentry = file->f_dentry;
		if (type == READ)
			fsnotify_access(dentry);
		else
			fsnotify_modify(dentry);
	}
	return ret;
}