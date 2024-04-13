void show_stack_regs(struct pt_regs *regs)
{
	show_trace_log_lvl(current, regs, NULL, KERN_DEFAULT);
}