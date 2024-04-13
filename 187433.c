static void *fill_in_one_cpu(struct mdesc_handle *hp, u64 mp, int cpuid,
			     void *arg)
{
	const u64 *cfreq = mdesc_get_property(hp, mp, "clock-frequency", NULL);
	struct trap_per_cpu *tb;
	cpuinfo_sparc *c;
	u64 a;

#ifndef CONFIG_SMP
	/* On uniprocessor we only want the values for the
	 * real physical cpu the kernel booted onto, however
	 * cpu_data() only has one entry at index 0.
	 */
	if (cpuid != real_hard_smp_processor_id())
		return NULL;
	cpuid = 0;
#endif

	c = &cpu_data(cpuid);
	c->clock_tick = *cfreq;

	tb = &trap_block[cpuid];
	get_mondo_data(hp, mp, tb);

	mdesc_for_each_arc(a, hp, mp, MDESC_ARC_TYPE_FWD) {
		u64 j, t = mdesc_arc_target(hp, a);
		const char *t_name;

		t_name = mdesc_node_name(hp, t);
		if (!strcmp(t_name, "cache")) {
			fill_in_one_cache(c, hp, t);
			continue;
		}

		mdesc_for_each_arc(j, hp, t, MDESC_ARC_TYPE_FWD) {
			u64 n = mdesc_arc_target(hp, j);
			const char *n_name;

			n_name = mdesc_node_name(hp, n);
			if (!strcmp(n_name, "cache"))
				fill_in_one_cache(c, hp, n);
		}
	}

	c->core_id = 0;
	c->proc_id = -1;

	return NULL;
}