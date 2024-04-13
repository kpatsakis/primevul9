static inline void blk_queue_enter_live(struct request_queue *q)
{
	/*
	 * Given that running in generic_make_request() context
	 * guarantees that a live reference against q_usage_counter has
	 * been established, further references under that same context
	 * need not check that the queue has been frozen (marked dead).
	 */
	percpu_ref_get(&q->q_usage_counter);
}