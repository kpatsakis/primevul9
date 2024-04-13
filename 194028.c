bool blk_mq_bio_list_merge(struct request_queue *q, struct list_head *list,
			   struct bio *bio)
{
	struct request *rq;
	int checked = 8;

	list_for_each_entry_reverse(rq, list, queuelist) {
		bool merged = false;

		if (!checked--)
			break;

		if (!blk_rq_merge_ok(rq, bio))
			continue;

		switch (blk_try_merge(rq, bio)) {
		case ELEVATOR_BACK_MERGE:
			if (blk_mq_sched_allow_merge(q, rq, bio))
				merged = bio_attempt_back_merge(q, rq, bio);
			break;
		case ELEVATOR_FRONT_MERGE:
			if (blk_mq_sched_allow_merge(q, rq, bio))
				merged = bio_attempt_front_merge(q, rq, bio);
			break;
		case ELEVATOR_DISCARD_MERGE:
			merged = bio_attempt_discard_merge(q, rq, bio);
			break;
		default:
			continue;
		}

		return merged;
	}

	return false;
}