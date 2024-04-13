void blk_mq_exit_sched(struct request_queue *q, struct elevator_queue *e)
{
	struct blk_mq_hw_ctx *hctx;
	unsigned int i;

	queue_for_each_hw_ctx(q, hctx, i) {
		blk_mq_debugfs_unregister_sched_hctx(hctx);
		if (e->type->ops.exit_hctx && hctx->sched_data) {
			e->type->ops.exit_hctx(hctx, i);
			hctx->sched_data = NULL;
		}
	}
	blk_mq_debugfs_unregister_sched(q);
	if (e->type->ops.exit_sched)
		e->type->ops.exit_sched(e);
	blk_mq_sched_tags_teardown(q);
	q->elevator = NULL;
}