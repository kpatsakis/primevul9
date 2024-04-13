void elv_rqhash_reposition(struct request_queue *q, struct request *rq)
{
	__elv_rqhash_del(rq);
	elv_rqhash_add(q, rq);
}