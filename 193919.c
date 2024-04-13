struct request *elv_latter_request(struct request_queue *q, struct request *rq)
{
	struct elevator_queue *e = q->elevator;

	if (e->type->ops.next_request)
		return e->type->ops.next_request(q, rq);

	return NULL;
}