static inline struct userfaultfd_wait_queue *find_userfault(
		struct userfaultfd_ctx *ctx)
{
	return find_userfault_in(&ctx->fault_pending_wqh);
}