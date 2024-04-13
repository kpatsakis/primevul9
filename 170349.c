static void bpf_prog_get_stats(const struct bpf_prog *prog,
			       struct bpf_prog_stats *stats)
{
	u64 nsecs = 0, cnt = 0, misses = 0;
	int cpu;

	for_each_possible_cpu(cpu) {
		const struct bpf_prog_stats *st;
		unsigned int start;
		u64 tnsecs, tcnt, tmisses;

		st = per_cpu_ptr(prog->stats, cpu);
		do {
			start = u64_stats_fetch_begin_irq(&st->syncp);
			tnsecs = st->nsecs;
			tcnt = st->cnt;
			tmisses = st->misses;
		} while (u64_stats_fetch_retry_irq(&st->syncp, start));
		nsecs += tnsecs;
		cnt += tcnt;
		misses += tmisses;
	}
	stats->nsecs = nsecs;
	stats->cnt = cnt;
	stats->misses = misses;
}