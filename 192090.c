static void mce_syscore_resume(void)
{
	__mcheck_cpu_init_generic();
	__mcheck_cpu_init_vendor(raw_cpu_ptr(&cpu_info));
	__mcheck_cpu_init_clear_banks();
}