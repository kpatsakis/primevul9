static void vgetcpu_cpu_init(void *arg)
{
	int cpu = smp_processor_id();
	struct desc_struct d = { };
	unsigned long node = 0;
#ifdef CONFIG_NUMA
	node = cpu_to_node(cpu);
#endif
	if (cpu_has(&cpu_data(cpu), X86_FEATURE_RDTSCP))
		write_rdtscp_aux((node << 12) | cpu);

	/*
	 * Store cpu number in limit so that it can be loaded
	 * quickly in user space in vgetcpu. (12 bits for the CPU
	 * and 8 bits for the node)
	 */
	d.limit0 = cpu | ((node & 0xf) << 12);
	d.limit = node >> 4;
	d.type = 5;		/* RO data, expand down, accessed */
	d.dpl = 3;		/* Visible to user code */
	d.s = 1;		/* Not a system segment */
	d.p = 1;		/* Present */
	d.d = 1;		/* 32-bit */

	write_gdt_entry(get_cpu_gdt_table(cpu), GDT_ENTRY_PER_CPU, &d, DESCTYPE_S);
}