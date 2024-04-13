static void svm_clear_vintr(struct vcpu_svm *svm)
{
	clr_intercept(svm, INTERCEPT_VINTR);

	svm->vmcb->control.int_ctl &= ~V_IRQ_MASK;
	mark_dirty(svm->vmcb, VMCB_INTR);
}