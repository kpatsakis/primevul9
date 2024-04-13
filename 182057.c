static inline void spin_time_accum_blocked(u64 start)
{
	u32 delta;

	delta = sched_clock() - start;
	__spin_time_accum(delta, spinlock_stats.histo_spin_blocked);
	spinlock_stats.time_blocked += delta;
}