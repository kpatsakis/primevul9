static inline int htab_lock_bucket(const struct bpf_htab *htab,
				   struct bucket *b, u32 hash,
				   unsigned long *pflags)
{
	unsigned long flags;

	hash = hash & HASHTAB_MAP_LOCK_MASK;

	migrate_disable();
	if (unlikely(__this_cpu_inc_return(*(htab->map_locked[hash])) != 1)) {
		__this_cpu_dec(*(htab->map_locked[hash]));
		migrate_enable();
		return -EBUSY;
	}

	if (htab_use_raw_lock(htab))
		raw_spin_lock_irqsave(&b->raw_lock, flags);
	else
		spin_lock_irqsave(&b->lock, flags);
	*pflags = flags;

	return 0;
}