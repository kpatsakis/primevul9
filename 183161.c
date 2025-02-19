static ssize_t aio_run_iocb(struct kiocb *req, unsigned opcode,
			    char __user *buf, bool compat)
{
	struct file *file = req->ki_filp;
	ssize_t ret;
	unsigned long nr_segs;
	int rw;
	fmode_t mode;
	aio_rw_op *rw_op;
	struct iovec inline_vec, *iovec = &inline_vec;

	switch (opcode) {
	case IOCB_CMD_PREAD:
	case IOCB_CMD_PREADV:
		mode	= FMODE_READ;
		rw	= READ;
		rw_op	= file->f_op->aio_read;
		goto rw_common;

	case IOCB_CMD_PWRITE:
	case IOCB_CMD_PWRITEV:
		mode	= FMODE_WRITE;
		rw	= WRITE;
		rw_op	= file->f_op->aio_write;
		goto rw_common;
rw_common:
		if (unlikely(!(file->f_mode & mode)))
			return -EBADF;

		if (!rw_op)
			return -EINVAL;

		ret = (opcode == IOCB_CMD_PREADV ||
		       opcode == IOCB_CMD_PWRITEV)
			? aio_setup_vectored_rw(req, rw, buf, &nr_segs,
						&iovec, compat)
			: aio_setup_single_vector(req, rw, buf, &nr_segs,
						  iovec);
		if (!ret)
			ret = rw_verify_area(rw, file, &req->ki_pos, req->ki_nbytes);
		if (ret < 0) {
			if (iovec != &inline_vec)
				kfree(iovec);
			return ret;
		}

		req->ki_nbytes = ret;

		/* XXX: move/kill - rw_verify_area()? */
		/* This matches the pread()/pwrite() logic */
		if (req->ki_pos < 0) {
			ret = -EINVAL;
			break;
		}

		if (rw == WRITE)
			file_start_write(file);

		ret = rw_op(req, iovec, nr_segs, req->ki_pos);

		if (rw == WRITE)
			file_end_write(file);
		break;

	case IOCB_CMD_FDSYNC:
		if (!file->f_op->aio_fsync)
			return -EINVAL;

		ret = file->f_op->aio_fsync(req, 1);
		break;

	case IOCB_CMD_FSYNC:
		if (!file->f_op->aio_fsync)
			return -EINVAL;

		ret = file->f_op->aio_fsync(req, 0);
		break;

	default:
		pr_debug("EINVAL: no operation provided\n");
		return -EINVAL;
	}

	if (iovec != &inline_vec)
		kfree(iovec);

	if (ret != -EIOCBQUEUED) {
		/*
		 * There's no easy way to restart the syscall since other AIO's
		 * may be already running. Just fail this IO with EINTR.
		 */
		if (unlikely(ret == -ERESTARTSYS || ret == -ERESTARTNOINTR ||
			     ret == -ERESTARTNOHAND ||
			     ret == -ERESTART_RESTARTBLOCK))
			ret = -EINTR;
		aio_complete(req, ret, 0);
	}

	return 0;
}