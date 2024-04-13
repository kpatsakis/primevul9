static inline void htab_unlock_bucket(const struct bpf_htab *htab,
				      struct bucket *b, u32 hash,
				      unsigned long flags)
{
	hash = hash & HASHTAB_MAP_LOCK_MASK;
	if (htab_use_raw_lock(htab))
		raw_spin_unlock_irqrestore(&b->raw_lock, flags);
	else
		spin_unlock_irqrestore(&b->lock, flags);
	__this_cpu_dec(*(htab->map_locked[hash]));
	migrate_enable();
}