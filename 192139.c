static void __print_mce(struct mce *m)
{
	pr_emerg(HW_ERR "CPU %d: Machine Check%s: %Lx Bank %d: %016Lx\n",
		 m->extcpu,
		 (m->mcgstatus & MCG_STATUS_MCIP ? " Exception" : ""),
		 m->mcgstatus, m->bank, m->status);

	if (m->ip) {
		pr_emerg(HW_ERR "RIP%s %02x:<%016Lx> ",
			!(m->mcgstatus & MCG_STATUS_EIPV) ? " !INEXACT!" : "",
			m->cs, m->ip);

		if (m->cs == __KERNEL_CS)
			pr_cont("{%pS}", (void *)(unsigned long)m->ip);
		pr_cont("\n");
	}

	pr_emerg(HW_ERR "TSC %llx ", m->tsc);
	if (m->addr)
		pr_cont("ADDR %llx ", m->addr);
	if (m->misc)
		pr_cont("MISC %llx ", m->misc);

	if (mce_flags.smca) {
		if (m->synd)
			pr_cont("SYND %llx ", m->synd);
		if (m->ipid)
			pr_cont("IPID %llx ", m->ipid);
	}

	pr_cont("\n");
	/*
	 * Note this output is parsed by external tools and old fields
	 * should not be changed.
	 */
	pr_emerg(HW_ERR "PROCESSOR %u:%x TIME %llu SOCKET %u APIC %x microcode %x\n",
		m->cpuvendor, m->cpuid, m->time, m->socketid, m->apicid,
		m->microcode);
}