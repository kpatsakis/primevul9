static void inet_frag_secret_rebuild(unsigned long dummy)
{
	struct inet_frags *f = (struct inet_frags *)dummy;
	unsigned long now = jiffies;
	int i;

	/* Per bucket lock NOT needed here, due to write lock protection */
	write_lock(&f->lock);

	get_random_bytes(&f->rnd, sizeof(u32));
	for (i = 0; i < INETFRAGS_HASHSZ; i++) {
		struct inet_frag_bucket *hb;
		struct inet_frag_queue *q;
		struct hlist_node *n;

		hb = &f->hash[i];
		hlist_for_each_entry_safe(q, n, &hb->chain, list) {
			unsigned int hval = f->hashfn(q);

			if (hval != i) {
				struct inet_frag_bucket *hb_dest;

				hlist_del(&q->list);

				/* Relink to new hash chain. */
				hb_dest = &f->hash[hval];
				hlist_add_head(&q->list, &hb_dest->chain);
			}
		}
	}
	write_unlock(&f->lock);

	mod_timer(&f->secret_timer, now + f->secret_interval);
}