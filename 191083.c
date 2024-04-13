static enum exit_fastpath_completion svm_exit_handlers_fastpath(struct kvm_vcpu *vcpu)
{
	if (!is_guest_mode(vcpu) &&
	    to_svm(vcpu)->vmcb->control.exit_code == SVM_EXIT_MSR &&
	    to_svm(vcpu)->vmcb->control.exit_info_1)
		return handle_fastpath_set_msr_irqoff(vcpu);

	return EXIT_FASTPATH_NONE;
}