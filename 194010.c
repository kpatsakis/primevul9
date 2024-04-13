static void blk_free_queue_rcu(struct rcu_head *rcu_head)
{
	struct request_queue *q = container_of(rcu_head, struct request_queue,
					       rcu_head);
	kmem_cache_free(blk_requestq_cachep, q);
}