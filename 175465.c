static void htab_init_buckets(struct bpf_htab *htab)
{
	unsigned i;

	for (i = 0; i < htab->n_buckets; i++) {
		INIT_HLIST_NULLS_HEAD(&htab->buckets[i].head, i);
		if (htab_use_raw_lock(htab)) {
			raw_spin_lock_init(&htab->buckets[i].raw_lock);
			lockdep_set_class(&htab->buckets[i].raw_lock,
					  &htab->lockdep_key);
		} else {
			spin_lock_init(&htab->buckets[i].lock);
			lockdep_set_class(&htab->buckets[i].lock,
					  &htab->lockdep_key);
		}
		cond_resched();
	}
}