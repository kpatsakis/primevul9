ssize_t wait_on_sync_kiocb(struct kiocb *req)
{
	while (!req->ki_ctx) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (req->ki_ctx)
			break;
		io_schedule();
	}
	__set_current_state(TASK_RUNNING);
	return req->ki_user_data;
}