static bool blk_mq_attempt_merge(struct request_queue *q,
				 struct blk_mq_hw_ctx *hctx,
				 struct blk_mq_ctx *ctx, struct bio *bio)
{
	enum hctx_type type = hctx->type;

	lockdep_assert_held(&ctx->lock);

	if (blk_mq_bio_list_merge(q, &ctx->rq_lists[type], bio)) {
		ctx->rq_merged++;
		return true;
	}

	return false;
}