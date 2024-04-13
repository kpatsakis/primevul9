SYSCALL_DEFINE1(io_destroy, aio_context_t, ctx)
{
	struct kioctx *ioctx = lookup_ioctx(ctx);
	if (likely(NULL != ioctx)) {
		struct completion requests_done =
			COMPLETION_INITIALIZER_ONSTACK(requests_done);

		/* Pass requests_done to kill_ioctx() where it can be set
		 * in a thread-safe way. If we try to set it here then we have
		 * a race condition if two io_destroy() called simultaneously.
		 */
		kill_ioctx(current->mm, ioctx, &requests_done);
		percpu_ref_put(&ioctx->users);

		/* Wait until all IO for the context are done. Otherwise kernel
		 * keep using user-space buffers even if user thinks the context
		 * is destroyed.
		 */
		wait_for_completion(&requests_done);

		return 0;
	}
	pr_debug("EINVAL: io_destroy: invalid context id\n");
	return -EINVAL;
}