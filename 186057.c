	__releases(&f->lock)
{
	struct inet_frag_bucket *hb;
	struct inet_frag_queue *q;
	int depth = 0;

	hb = &f->hash[hash];

	spin_lock(&hb->chain_lock);
	hlist_for_each_entry(q, &hb->chain, list) {
		if (q->net == nf && f->match(q, key)) {
			atomic_inc(&q->refcnt);
			spin_unlock(&hb->chain_lock);
			read_unlock(&f->lock);
			return q;
		}
		depth++;
	}
	spin_unlock(&hb->chain_lock);
	read_unlock(&f->lock);

	if (depth <= INETFRAGS_MAXDEPTH)
		return inet_frag_create(nf, f, key);
	else
		return ERR_PTR(-ENOBUFS);
}