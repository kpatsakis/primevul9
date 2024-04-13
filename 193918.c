enum elv_merge elv_merge(struct request_queue *q, struct request **req,
		struct bio *bio)
{
	struct elevator_queue *e = q->elevator;
	struct request *__rq;

	/*
	 * Levels of merges:
	 * 	nomerges:  No merges at all attempted
	 * 	noxmerges: Only simple one-hit cache try
	 * 	merges:	   All merge tries attempted
	 */
	if (blk_queue_nomerges(q) || !bio_mergeable(bio))
		return ELEVATOR_NO_MERGE;

	/*
	 * First try one-hit cache.
	 */
	if (q->last_merge && elv_bio_merge_ok(q->last_merge, bio)) {
		enum elv_merge ret = blk_try_merge(q->last_merge, bio);

		if (ret != ELEVATOR_NO_MERGE) {
			*req = q->last_merge;
			return ret;
		}
	}

	if (blk_queue_noxmerges(q))
		return ELEVATOR_NO_MERGE;

	/*
	 * See if our hash lookup can find a potential backmerge.
	 */
	__rq = elv_rqhash_find(q, bio->bi_iter.bi_sector);
	if (__rq && elv_bio_merge_ok(__rq, bio)) {
		*req = __rq;
		return ELEVATOR_BACK_MERGE;
	}

	if (e->type->ops.request_merge)
		return e->type->ops.request_merge(q, req, bio);

	return ELEVATOR_NO_MERGE;
}