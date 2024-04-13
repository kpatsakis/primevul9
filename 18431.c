static void do_numa_crng_init(struct work_struct *work)
{
	int i;
	struct crng_state *crng;
	struct crng_state **pool;

	pool = kcalloc(nr_node_ids, sizeof(*pool), GFP_KERNEL|__GFP_NOFAIL);
	for_each_online_node(i) {
		crng = kmalloc_node(sizeof(struct crng_state),
				    GFP_KERNEL | __GFP_NOFAIL, i);
		spin_lock_init(&crng->lock);
		crng_initialize_secondary(crng);
		pool[i] = crng;
	}
	mb();
	if (cmpxchg(&crng_node_pool, NULL, pool)) {
		for_each_node(i)
			kfree(pool[i]);
		kfree(pool);
	}
}