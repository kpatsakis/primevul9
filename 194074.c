static inline bool elv_support_iosched(struct request_queue *q)
{
	if (q->tag_set && (q->tag_set->flags & BLK_MQ_F_NO_SCHED))
		return false;
	return true;
}