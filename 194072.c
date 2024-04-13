struct request *elv_former_request(struct request_queue *q, struct request *rq)
{
	struct elevator_queue *e = q->elevator;

	if (e->type->ops.former_request)
		return e->type->ops.former_request(q, rq);

	return NULL;
}