static void pcpu_init_value(struct bpf_htab *htab, void __percpu *pptr,
			    void *value, bool onallcpus)
{
	/* When using prealloc and not setting the initial value on all cpus,
	 * zero-fill element values for other cpus (just as what happens when
	 * not using prealloc). Otherwise, bpf program has no way to ensure
	 * known initial values for cpus other than current one
	 * (onallcpus=false always when coming from bpf prog).
	 */
	if (htab_is_prealloc(htab) && !onallcpus) {
		u32 size = round_up(htab->map.value_size, 8);
		int current_cpu = raw_smp_processor_id();
		int cpu;

		for_each_possible_cpu(cpu) {
			if (cpu == current_cpu)
				bpf_long_memcpy(per_cpu_ptr(pptr, cpu), value,
						size);
			else
				memset(per_cpu_ptr(pptr, cpu), 0, size);
		}
	} else {
		pcpu_copy_value(htab, pptr, value, onallcpus);
	}
}