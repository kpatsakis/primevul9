static void userfaultfd_ctx_get(struct userfaultfd_ctx *ctx)
{
	if (!atomic_inc_not_zero(&ctx->refcount))
		BUG();
}