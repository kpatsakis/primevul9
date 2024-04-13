static void free_ioctx(struct work_struct *work)
{
	struct kioctx *ctx = container_of(work, struct kioctx, free_work);

	pr_debug("freeing %p\n", ctx);

	aio_free_ring(ctx);
	free_percpu(ctx->cpu);
	kmem_cache_free(kioctx_cachep, ctx);
}