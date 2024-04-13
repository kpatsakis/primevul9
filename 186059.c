int inet_frag_evictor(struct netns_frags *nf, struct inet_frags *f, bool force)
{
	struct inet_frag_queue *q;
	int work, evicted = 0;

	if (!force) {
		if (frag_mem_limit(nf) <= nf->high_thresh)
			return 0;
	}

	work = frag_mem_limit(nf) - nf->low_thresh;
	while (work > 0) {
		spin_lock(&nf->lru_lock);

		if (list_empty(&nf->lru_list)) {
			spin_unlock(&nf->lru_lock);
			break;
		}

		q = list_first_entry(&nf->lru_list,
				struct inet_frag_queue, lru_list);
		atomic_inc(&q->refcnt);
		/* Remove q from list to avoid several CPUs grabbing it */
		list_del_init(&q->lru_list);

		spin_unlock(&nf->lru_lock);

		spin_lock(&q->lock);
		if (!(q->last_in & INET_FRAG_COMPLETE))
			inet_frag_kill(q, f);
		spin_unlock(&q->lock);

		if (atomic_dec_and_test(&q->refcnt))
			inet_frag_destroy(q, f, &work);
		evicted++;
	}

	return evicted;
}