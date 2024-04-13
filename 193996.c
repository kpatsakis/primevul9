void blk_mq_sched_insert_request(struct request *rq, bool at_head,
				 bool run_queue, bool async)
{
	struct request_queue *q = rq->q;
	struct elevator_queue *e = q->elevator;
	struct blk_mq_ctx *ctx = rq->mq_ctx;
	struct blk_mq_hw_ctx *hctx = rq->mq_hctx;

	/* flush rq in flush machinery need to be dispatched directly */
	if (!(rq->rq_flags & RQF_FLUSH_SEQ) && op_is_flush(rq->cmd_flags)) {
		blk_insert_flush(rq);
		goto run;
	}

	WARN_ON(e && (rq->tag != -1));

	if (blk_mq_sched_bypass_insert(hctx, !!e, rq))
		goto run;

	if (e && e->type->ops.insert_requests) {
		LIST_HEAD(list);

		list_add(&rq->queuelist, &list);
		e->type->ops.insert_requests(hctx, &list, at_head);
	} else {
		spin_lock(&ctx->lock);
		__blk_mq_insert_request(hctx, rq, at_head);
		spin_unlock(&ctx->lock);
	}

run:
	if (run_queue)
		blk_mq_run_hw_queue(hctx, async);
}