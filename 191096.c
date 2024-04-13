static inline void svm_enable_vintr(struct vcpu_svm *svm)
{
	struct vmcb_control_area *control;

	/* The following fields are ignored when AVIC is enabled */
	WARN_ON(kvm_vcpu_apicv_active(&svm->vcpu));

	/*
	 * This is just a dummy VINTR to actually cause a vmexit to happen.
	 * Actual injection of virtual interrupts happens through EVENTINJ.
	 */
	control = &svm->vmcb->control;
	control->int_vector = 0x0;
	control->int_ctl &= ~V_INTR_PRIO_MASK;
	control->int_ctl |= V_IRQ_MASK |
		((/*control->int_vector >> 4*/ 0xf) << V_INTR_PRIO_SHIFT);
	mark_dirty(svm->vmcb, VMCB_INTR);
}