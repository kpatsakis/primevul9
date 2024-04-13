static void *record_one_cpu(struct mdesc_handle *hp, u64 mp, int cpuid,
			    void *arg)
{
	ncpus_probed++;
#ifdef CONFIG_SMP
	set_cpu_present(cpuid, true);
#endif
	return NULL;
}