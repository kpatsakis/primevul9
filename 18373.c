static void mix_pool_bytes(struct entropy_store *r, const void *in,
			   int nbytes)
{
	unsigned long flags;

	trace_mix_pool_bytes(r->name, nbytes, _RET_IP_);
	spin_lock_irqsave(&r->lock, flags);
	_mix_pool_bytes(r, in, nbytes);
	spin_unlock_irqrestore(&r->lock, flags);
}