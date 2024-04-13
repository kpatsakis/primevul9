static inline void fq_unlink(struct inet_frag_queue *fq, struct inet_frags *f)
{
	struct inet_frag_bucket *hb;
	unsigned int hash;

	read_lock(&f->lock);
	hash = f->hashfn(fq);
	hb = &f->hash[hash];

	spin_lock(&hb->chain_lock);
	hlist_del(&fq->list);
	spin_unlock(&hb->chain_lock);

	read_unlock(&f->lock);
	inet_frag_lru_del(fq);
}