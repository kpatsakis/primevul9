do_page_fault(struct pt_regs *regs, unsigned long error_code)
{
	unsigned long address = read_cr2(); /* Get the faulting address */
	enum ctx_state prev_state;

	prev_state = exception_enter();
	if (trace_pagefault_enabled())
		trace_page_fault_entries(address, regs, error_code);

	__do_page_fault(regs, error_code, address);
	exception_exit(prev_state);
}