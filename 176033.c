static void init_once_userfaultfd_ctx(void *mem)
{
	struct userfaultfd_ctx *ctx = (struct userfaultfd_ctx *) mem;

	init_waitqueue_head(&ctx->fault_pending_wqh);
	init_waitqueue_head(&ctx->fault_wqh);
	init_waitqueue_head(&ctx->event_wqh);
	init_waitqueue_head(&ctx->fd_wqh);
	seqcount_init(&ctx->refile_seq);
}