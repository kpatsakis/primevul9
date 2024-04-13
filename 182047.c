do_async_page_fault(struct pt_regs *regs, unsigned long error_code)
{
	enum ctx_state prev_state;

	switch (kvm_read_and_reset_pf_reason()) {
	default:
		trace_do_page_fault(regs, error_code);
		break;
	case KVM_PV_REASON_PAGE_NOT_PRESENT:
		/* page is swapped out by the host. */
		prev_state = exception_enter();
		exit_idle();
		kvm_async_pf_task_wait((u32)read_cr2());
		exception_exit(prev_state);
		break;
	case KVM_PV_REASON_PAGE_READY:
		rcu_irq_enter();
		exit_idle();
		kvm_async_pf_task_wake((u32)read_cr2());
		rcu_irq_exit();
		break;
	}
}