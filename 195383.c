void show_regs(struct pt_regs *regs)
{
	bool all = true;

	show_regs_print_info(KERN_DEFAULT);

	if (IS_ENABLED(CONFIG_X86_32))
		all = !user_mode(regs);

	__show_regs(regs, all);

	/*
	 * When in-kernel, we also print out the stack at the time of the fault..
	 */
	if (!user_mode(regs))
		show_trace_log_lvl(current, regs, NULL, KERN_DEFAULT);
}