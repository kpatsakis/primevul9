void mce_setup(struct mce *m)
{
	memset(m, 0, sizeof(struct mce));
	m->cpu = m->extcpu = smp_processor_id();
	/* We hope get_seconds stays lockless */
	m->time = get_seconds();
	m->cpuvendor = boot_cpu_data.x86_vendor;
	m->cpuid = cpuid_eax(1);
	m->socketid = cpu_data(m->extcpu).phys_proc_id;
	m->apicid = cpu_data(m->extcpu).initial_apicid;
	rdmsrl(MSR_IA32_MCG_CAP, m->mcgcap);

	if (this_cpu_has(X86_FEATURE_INTEL_PPIN))
		rdmsrl(MSR_PPIN, m->ppin);

	m->microcode = boot_cpu_data.microcode;
}