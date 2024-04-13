static int io_submit_one(struct kioctx *ctx, struct iocb __user *user_iocb,
			 struct iocb *iocb, bool compat)
{
	struct kiocb *req;
	ssize_t ret;

	/* enforce forwards compatibility on users */
	if (unlikely(iocb->aio_reserved1 || iocb->aio_reserved2)) {
		pr_debug("EINVAL: reserve field set\n");
		return -EINVAL;
	}

	/* prevent overflows */
	if (unlikely(
	    (iocb->aio_buf != (unsigned long)iocb->aio_buf) ||
	    (iocb->aio_nbytes != (size_t)iocb->aio_nbytes) ||
	    ((ssize_t)iocb->aio_nbytes < 0)
	   )) {
		pr_debug("EINVAL: io_submit: overflow check\n");
		return -EINVAL;
	}

	req = aio_get_req(ctx);
	if (unlikely(!req))
		return -EAGAIN;

	req->ki_filp = fget(iocb->aio_fildes);
	if (unlikely(!req->ki_filp)) {
		ret = -EBADF;
		goto out_put_req;
	}

	if (iocb->aio_flags & IOCB_FLAG_RESFD) {
		/*
		 * If the IOCB_FLAG_RESFD flag of aio_flags is set, get an
		 * instance of the file* now. The file descriptor must be
		 * an eventfd() fd, and will be signaled for each completed
		 * event using the eventfd_signal() function.
		 */
		req->ki_eventfd = eventfd_ctx_fdget((int) iocb->aio_resfd);
		if (IS_ERR(req->ki_eventfd)) {
			ret = PTR_ERR(req->ki_eventfd);
			req->ki_eventfd = NULL;
			goto out_put_req;
		}
	}

	ret = put_user(KIOCB_KEY, &user_iocb->aio_key);
	if (unlikely(ret)) {
		pr_debug("EFAULT: aio_key\n");
		goto out_put_req;
	}

	req->ki_obj.user = user_iocb;
	req->ki_user_data = iocb->aio_data;
	req->ki_pos = iocb->aio_offset;
	req->ki_nbytes = iocb->aio_nbytes;

	ret = aio_run_iocb(req, iocb->aio_lio_opcode,
			   (char __user *)(unsigned long)iocb->aio_buf,
			   compat);
	if (ret)
		goto out_put_req;

	return 0;
out_put_req:
	put_reqs_available(ctx, 1);
	percpu_ref_put(&ctx->reqs);
	kiocb_free(req);
	return ret;
}