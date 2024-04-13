void oops_end(unsigned long flags, struct pt_regs *regs, int signr)
{
	if (regs && kexec_should_crash(current))
		crash_kexec(regs);

	bust_spinlocks(0);
	die_owner = -1;
	add_taint(TAINT_DIE, LOCKDEP_NOW_UNRELIABLE);
	die_nest_count--;
	if (!die_nest_count)
		/* Nest count reaches zero, release the lock. */
		arch_spin_unlock(&die_lock);
	raw_local_irq_restore(flags);
	oops_exit();

	/* Executive summary in case the oops scrolled away */
	__show_regs(&exec_summary_regs, true);

	if (!signr)
		return;
	if (in_interrupt())
		panic("Fatal exception in interrupt");
	if (panic_on_oops)
		panic("Fatal exception");

	/*
	 * We're not going to return, but we might be on an IST stack or
	 * have very little stack space left.  Rewind the stack and kill
	 * the task.
	 * Before we rewind the stack, we have to tell KASAN that we're going to
	 * reuse the task stack and that existing poisons are invalid.
	 */
	kasan_unpoison_task_stack(current);
	rewind_stack_do_exit(signr);
}