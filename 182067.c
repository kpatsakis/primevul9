static inline void add_stats(enum kvm_contention_stat var, u32 val)
{
	check_zero();
	spinlock_stats.contention_stats[var] += val;
}