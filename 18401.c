int timers_prepare_cpu(unsigned int cpu)
{
	struct timer_base *base;
	int b;

	for (b = 0; b < NR_BASES; b++) {
		base = per_cpu_ptr(&timer_bases[b], cpu);
		base->clk = jiffies;
		base->next_expiry = base->clk + NEXT_TIMER_MAX_DELTA;
		base->is_idle = false;
		base->must_forward_clk = true;
	}
	return 0;
}