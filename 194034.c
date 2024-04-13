void blk_mq_sched_assign_ioc(struct request *rq)
{
	struct request_queue *q = rq->q;
	struct io_context *ioc;
	struct io_cq *icq;

	/*
	 * May not have an IO context if it's a passthrough request
	 */
	ioc = current->io_context;
	if (!ioc)
		return;

	spin_lock_irq(&q->queue_lock);
	icq = ioc_lookup_icq(ioc, q);
	spin_unlock_irq(&q->queue_lock);

	if (!icq) {
		icq = ioc_create_icq(ioc, q, GFP_ATOMIC);
		if (!icq)
			return;
	}
	get_io_context(icq->ioc);
	rq->elv.icq = icq;
}