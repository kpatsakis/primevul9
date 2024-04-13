static unsigned int userfaultfd_poll(struct file *file, poll_table *wait)
{
	struct userfaultfd_ctx *ctx = file->private_data;
	unsigned int ret;

	poll_wait(file, &ctx->fd_wqh, wait);

	switch (ctx->state) {
	case UFFD_STATE_WAIT_API:
		return POLLERR;
	case UFFD_STATE_RUNNING:
		/*
		 * poll() never guarantees that read won't block.
		 * userfaults can be waken before they're read().
		 */
		if (unlikely(!(file->f_flags & O_NONBLOCK)))
			return POLLERR;
		/*
		 * lockless access to see if there are pending faults
		 * __pollwait last action is the add_wait_queue but
		 * the spin_unlock would allow the waitqueue_active to
		 * pass above the actual list_add inside
		 * add_wait_queue critical section. So use a full
		 * memory barrier to serialize the list_add write of
		 * add_wait_queue() with the waitqueue_active read
		 * below.
		 */
		ret = 0;
		smp_mb();
		if (waitqueue_active(&ctx->fault_pending_wqh))
			ret = POLLIN;
		else if (waitqueue_active(&ctx->event_wqh))
			ret = POLLIN;

		return ret;
	default:
		WARN_ON_ONCE(1);
		return POLLERR;
	}
}