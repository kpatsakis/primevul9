static long userfaultfd_ioctl(struct file *file, unsigned cmd,
			      unsigned long arg)
{
	int ret = -EINVAL;
	struct userfaultfd_ctx *ctx = file->private_data;

	if (cmd != UFFDIO_API && ctx->state == UFFD_STATE_WAIT_API)
		return -EINVAL;

	switch(cmd) {
	case UFFDIO_API:
		ret = userfaultfd_api(ctx, arg);
		break;
	case UFFDIO_REGISTER:
		ret = userfaultfd_register(ctx, arg);
		break;
	case UFFDIO_UNREGISTER:
		ret = userfaultfd_unregister(ctx, arg);
		break;
	case UFFDIO_WAKE:
		ret = userfaultfd_wake(ctx, arg);
		break;
	case UFFDIO_COPY:
		ret = userfaultfd_copy(ctx, arg);
		break;
	case UFFDIO_ZEROPAGE:
		ret = userfaultfd_zeropage(ctx, arg);
		break;
	}
	return ret;
}