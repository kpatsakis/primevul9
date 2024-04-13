static void pcpu_copy_value(struct bpf_htab *htab, void __percpu *pptr,
			    void *value, bool onallcpus)
{
	if (!onallcpus) {
		/* copy true value_size bytes */
		memcpy(this_cpu_ptr(pptr), value, htab->map.value_size);
	} else {
		u32 size = round_up(htab->map.value_size, 8);
		int off = 0, cpu;

		for_each_possible_cpu(cpu) {
			bpf_long_memcpy(per_cpu_ptr(pptr, cpu),
					value + off, size);
			off += size;
		}
	}
}