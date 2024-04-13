static void free_ioctx_reqs(struct percpu_ref *ref)
{
	struct kioctx *ctx = container_of(ref, struct kioctx, reqs);

	/* At this point we know that there are no any in-flight requests */
	if (ctx->requests_done)
		complete(ctx->requests_done);

	INIT_WORK(&ctx->free_work, free_ioctx);
	schedule_work(&ctx->free_work);
}