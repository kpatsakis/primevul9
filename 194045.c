bool blk_mq_sched_try_insert_merge(struct request_queue *q, struct request *rq)
{
	return rq_mergeable(rq) && elv_attempt_insert_merge(q, rq);
}