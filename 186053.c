void inet_frags_init(struct inet_frags *f)
{
	int i;

	for (i = 0; i < INETFRAGS_HASHSZ; i++) {
		struct inet_frag_bucket *hb = &f->hash[i];

		spin_lock_init(&hb->chain_lock);
		INIT_HLIST_HEAD(&hb->chain);
	}
	rwlock_init(&f->lock);

	setup_timer(&f->secret_timer, inet_frag_secret_rebuild,
			(unsigned long)f);
	f->secret_timer.expires = jiffies + f->secret_interval;
	add_timer(&f->secret_timer);
}