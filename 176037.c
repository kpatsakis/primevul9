static ssize_t userfaultfd_read(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct userfaultfd_ctx *ctx = file->private_data;
	ssize_t _ret, ret = 0;
	struct uffd_msg msg;
	int no_wait = file->f_flags & O_NONBLOCK;

	if (ctx->state == UFFD_STATE_WAIT_API)
		return -EINVAL;

	for (;;) {
		if (count < sizeof(msg))
			return ret ? ret : -EINVAL;
		_ret = userfaultfd_ctx_read(ctx, no_wait, &msg);
		if (_ret < 0)
			return ret ? ret : _ret;
		if (copy_to_user((__u64 __user *) buf, &msg, sizeof(msg)))
			return ret ? ret : -EFAULT;
		ret += sizeof(msg);
		buf += sizeof(msg);
		count -= sizeof(msg);
		/*
		 * Allow to read more than one fault at time but only
		 * block if waiting for the very first one.
		 */
		no_wait = O_NONBLOCK;
	}
}