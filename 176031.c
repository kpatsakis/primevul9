static inline struct userfaultfd_wait_queue *find_userfault_evt(
		struct userfaultfd_ctx *ctx)
{
	return find_userfault_in(&ctx->event_wqh);
}