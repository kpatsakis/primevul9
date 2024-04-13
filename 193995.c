static int elv_iosched_allow_bio_merge(struct request *rq, struct bio *bio)
{
	struct request_queue *q = rq->q;
	struct elevator_queue *e = q->elevator;

	if (e->type->ops.allow_merge)
		return e->type->ops.allow_merge(q, rq, bio);

	return 1;
}