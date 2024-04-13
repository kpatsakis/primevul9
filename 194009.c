static struct blk_mq_ctx *blk_mq_next_ctx(struct blk_mq_hw_ctx *hctx,
					  struct blk_mq_ctx *ctx)
{
	unsigned short idx = ctx->index_hw[hctx->type];

	if (++idx == hctx->nr_ctx)
		idx = 0;

	return hctx->ctxs[idx];
}