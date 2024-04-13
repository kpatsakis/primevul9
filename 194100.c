static void blk_mq_do_dispatch_sched(struct blk_mq_hw_ctx *hctx)
{
	struct request_queue *q = hctx->queue;
	struct elevator_queue *e = q->elevator;
	LIST_HEAD(rq_list);

	do {
		struct request *rq;

		if (e->type->ops.has_work && !e->type->ops.has_work(hctx))
			break;

		if (!blk_mq_get_dispatch_budget(hctx))
			break;

		rq = e->type->ops.dispatch_request(hctx);
		if (!rq) {
			blk_mq_put_dispatch_budget(hctx);
			break;
		}

		/*
		 * Now this rq owns the budget which has to be released
		 * if this rq won't be queued to driver via .queue_rq()
		 * in blk_mq_dispatch_rq_list().
		 */
		list_add(&rq->queuelist, &rq_list);
	} while (blk_mq_dispatch_rq_list(q, &rq_list, true));
}