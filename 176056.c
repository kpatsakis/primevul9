static void __wake_userfault(struct userfaultfd_ctx *ctx,
			     struct userfaultfd_wake_range *range)
{
	spin_lock(&ctx->fault_pending_wqh.lock);
	/* wake all in the range and autoremove */
	if (waitqueue_active(&ctx->fault_pending_wqh))
		__wake_up_locked_key(&ctx->fault_pending_wqh, TASK_NORMAL,
				     range);
	if (waitqueue_active(&ctx->fault_wqh))
		__wake_up_locked_key(&ctx->fault_wqh, TASK_NORMAL, range);
	spin_unlock(&ctx->fault_pending_wqh.lock);
}