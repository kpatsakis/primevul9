bool __blk_mq_sched_bio_merge(struct request_queue *q, struct bio *bio)
{
	struct elevator_queue *e = q->elevator;
	struct blk_mq_ctx *ctx = blk_mq_get_ctx(q);
	struct blk_mq_hw_ctx *hctx = blk_mq_map_queue(q, bio->bi_opf, ctx);
	bool ret = false;
	enum hctx_type type;

	if (e && e->type->ops.bio_merge) {
		blk_mq_put_ctx(ctx);
		return e->type->ops.bio_merge(hctx, bio);
	}

	type = hctx->type;
	if ((hctx->flags & BLK_MQ_F_SHOULD_MERGE) &&
			!list_empty_careful(&ctx->rq_lists[type])) {
		/* default per sw-queue merge */
		spin_lock(&ctx->lock);
		ret = blk_mq_attempt_merge(q, hctx, ctx, bio);
		spin_unlock(&ctx->lock);
	}

	blk_mq_put_ctx(ctx);
	return ret;
}