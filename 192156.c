static void unexpected_machine_check(struct pt_regs *regs, long error_code)
{
	pr_err("CPU#%d: Unexpected int18 (Machine Check)\n",
	       smp_processor_id());
}