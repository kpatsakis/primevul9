void blk_mq_sched_request_inserted(struct request *rq)
{
	trace_block_rq_insert(rq->q, rq);
}