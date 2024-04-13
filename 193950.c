struct request *blk_get_request(struct request_queue *q, unsigned int op,
				blk_mq_req_flags_t flags)
{
	struct request *req;

	WARN_ON_ONCE(op & REQ_NOWAIT);
	WARN_ON_ONCE(flags & ~(BLK_MQ_REQ_NOWAIT | BLK_MQ_REQ_PREEMPT));

	req = blk_mq_alloc_request(q, op, flags);
	if (!IS_ERR(req) && q->mq_ops->initialize_rq_fn)
		q->mq_ops->initialize_rq_fn(req);

	return req;
}