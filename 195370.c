mm_fault_error(struct pt_regs *regs, unsigned long error_code,
	       unsigned long address, u32 *pkey, vm_fault_t fault)
{
	if (fatal_signal_pending(current) && !(error_code & X86_PF_USER)) {
		no_context(regs, error_code, address, 0, 0);
		return;
	}

	if (fault & VM_FAULT_OOM) {
		/* Kernel mode? Handle exceptions or die: */
		if (!(error_code & X86_PF_USER)) {
			no_context(regs, error_code, address,
				   SIGSEGV, SEGV_MAPERR);
			return;
		}

		/*
		 * We ran out of memory, call the OOM killer, and return the
		 * userspace (which will retry the fault, or kill us if we got
		 * oom-killed):
		 */
		pagefault_out_of_memory();
	} else {
		if (fault & (VM_FAULT_SIGBUS|VM_FAULT_HWPOISON|
			     VM_FAULT_HWPOISON_LARGE))
			do_sigbus(regs, error_code, address, pkey, fault);
		else if (fault & VM_FAULT_SIGSEGV)
			bad_area_nosemaphore(regs, error_code, address, pkey);
		else
			BUG();
	}
}