bool elv_attempt_insert_merge(struct request_queue *q, struct request *rq)
{
	struct request *__rq;
	bool ret;

	if (blk_queue_nomerges(q))
		return false;

	/*
	 * First try one-hit cache.
	 */
	if (q->last_merge && blk_attempt_req_merge(q, q->last_merge, rq))
		return true;

	if (blk_queue_noxmerges(q))
		return false;

	ret = false;
	/*
	 * See if our hash lookup can find a potential backmerge.
	 */
	while (1) {
		__rq = elv_rqhash_find(q, blk_rq_pos(rq));
		if (!__rq || !blk_attempt_req_merge(q, __rq, rq))
			break;

		/* The merged request could be merged with others, try again */
		ret = true;
		rq = __rq;
	}

	return ret;
}