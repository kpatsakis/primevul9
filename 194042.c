struct request *elv_rqhash_find(struct request_queue *q, sector_t offset)
{
	struct elevator_queue *e = q->elevator;
	struct hlist_node *next;
	struct request *rq;

	hash_for_each_possible_safe(e->hash, rq, next, hash, offset) {
		BUG_ON(!ELV_ON_HASH(rq));

		if (unlikely(!rq_mergeable(rq))) {
			__elv_rqhash_del(rq);
			continue;
		}

		if (rq_hash_key(rq) == offset)
			return rq;
	}

	return NULL;
}