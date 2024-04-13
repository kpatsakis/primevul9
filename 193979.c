void blk_mq_sched_insert_requests(struct blk_mq_hw_ctx *hctx,
				  struct blk_mq_ctx *ctx,
				  struct list_head *list, bool run_queue_async)
{
	struct elevator_queue *e;
	struct request_queue *q = hctx->queue;

	/*
	 * blk_mq_sched_insert_requests() is called from flush plug
	 * context only, and hold one usage counter to prevent queue
	 * from being released.
	 */
	percpu_ref_get(&q->q_usage_counter);

	e = hctx->queue->elevator;
	if (e && e->type->ops.insert_requests)
		e->type->ops.insert_requests(hctx, list, false);
	else {
		/*
		 * try to issue requests directly if the hw queue isn't
		 * busy in case of 'none' scheduler, and this way may save
		 * us one extra enqueue & dequeue to sw queue.
		 */
		if (!hctx->dispatch_busy && !e && !run_queue_async) {
			blk_mq_try_issue_list_directly(hctx, list);
			if (list_empty(list))
				goto out;
		}
		blk_mq_insert_requests(hctx, ctx, list);
	}

	blk_mq_run_hw_queue(hctx, run_queue_async);
 out:
	percpu_ref_put(&q->q_usage_counter);
}