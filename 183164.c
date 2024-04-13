static inline struct kiocb *aio_get_req(struct kioctx *ctx)
{
	struct kiocb *req;

	if (!get_reqs_available(ctx)) {
		user_refill_reqs_available(ctx);
		if (!get_reqs_available(ctx))
			return NULL;
	}

	req = kmem_cache_alloc(kiocb_cachep, GFP_KERNEL|__GFP_ZERO);
	if (unlikely(!req))
		goto out_put;

	percpu_ref_get(&ctx->reqs);

	req->ki_ctx = ctx;
	return req;
out_put:
	put_reqs_available(ctx, 1);
	return NULL;
}