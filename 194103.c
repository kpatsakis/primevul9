void blk_queue_exit(struct request_queue *q)
{
	percpu_ref_put(&q->q_usage_counter);
}