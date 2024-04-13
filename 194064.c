void blk_mq_sched_free_requests(struct request_queue *q)
{
	struct blk_mq_hw_ctx *hctx;
	int i;

	lockdep_assert_held(&q->sysfs_lock);
	WARN_ON(!q->elevator);

	queue_for_each_hw_ctx(q, hctx, i) {
		if (hctx->sched_tags)
			blk_mq_free_rqs(q->tag_set, hctx->sched_tags, i);
	}
}