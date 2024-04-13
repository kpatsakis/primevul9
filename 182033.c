static inline void check_zero(void)
{
	u8 ret;
	u8 old;

	old = ACCESS_ONCE(zero_stats);
	if (unlikely(old)) {
		ret = cmpxchg(&zero_stats, old, 0);
		/* This ensures only one fellow resets the stat */
		if (ret == old)
			memset(&spinlock_stats, 0, sizeof(spinlock_stats));
	}
}