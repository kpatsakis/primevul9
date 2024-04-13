static void show_regs_if_on_stack(struct stack_info *info, struct pt_regs *regs,
				  bool partial)
{
	/*
	 * These on_stack() checks aren't strictly necessary: the unwind code
	 * has already validated the 'regs' pointer.  The checks are done for
	 * ordering reasons: if the registers are on the next stack, we don't
	 * want to print them out yet.  Otherwise they'll be shown as part of
	 * the wrong stack.  Later, when show_trace_log_lvl() switches to the
	 * next stack, this function will be called again with the same regs so
	 * they can be printed in the right context.
	 */
	if (!partial && on_stack(info, regs, sizeof(*regs))) {
		__show_regs(regs, 0);

	} else if (partial && on_stack(info, (void *)regs + IRET_FRAME_OFFSET,
				       IRET_FRAME_SIZE)) {
		/*
		 * When an interrupt or exception occurs in entry code, the
		 * full pt_regs might not have been saved yet.  In that case
		 * just print the iret frame.
		 */
		show_iret_regs(regs);
	}
}