bool blk_attempt_plug_merge(struct request_queue *q, struct bio *bio,
			    struct request **same_queue_rq)
{
	struct blk_plug *plug;
	struct request *rq;
	struct list_head *plug_list;

	plug = current->plug;
	if (!plug)
		return false;

	plug_list = &plug->mq_list;

	list_for_each_entry_reverse(rq, plug_list, queuelist) {
		bool merged = false;

		if (rq->q == q && same_queue_rq) {
			/*
			 * Only blk-mq multiple hardware queues case checks the
			 * rq in the same queue, there should be only one such
			 * rq in a queue
			 **/
			*same_queue_rq = rq;
		}

		if (rq->q != q || !blk_rq_merge_ok(rq, bio))
			continue;

		switch (blk_try_merge(rq, bio)) {
		case ELEVATOR_BACK_MERGE:
			merged = bio_attempt_back_merge(q, rq, bio);
			break;
		case ELEVATOR_FRONT_MERGE:
			merged = bio_attempt_front_merge(q, rq, bio);
			break;
		case ELEVATOR_DISCARD_MERGE:
			merged = bio_attempt_discard_merge(q, rq, bio);
			break;
		default:
			break;
		}

		if (merged)
			return true;
	}

	return false;
}