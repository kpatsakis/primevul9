static void nested_svm_inject_exception_vmexit(struct vcpu_svm *svm)
{
	unsigned int nr = svm->vcpu.arch.exception.nr;

	svm->vmcb->control.exit_code = SVM_EXIT_EXCP_BASE + nr;
	svm->vmcb->control.exit_code_hi = 0;

	if (svm->vcpu.arch.exception.has_error_code)
		svm->vmcb->control.exit_info_1 = svm->vcpu.arch.exception.error_code;

	/*
	 * EXITINFO2 is undefined for all exception intercepts other
	 * than #PF.
	 */
	if (nr == PF_VECTOR) {
		if (svm->vcpu.arch.exception.nested_apf)
			svm->vmcb->control.exit_info_2 = svm->vcpu.arch.apf.nested_apf_token;
		else if (svm->vcpu.arch.exception.has_payload)
			svm->vmcb->control.exit_info_2 = svm->vcpu.arch.exception.payload;
		else
			svm->vmcb->control.exit_info_2 = svm->vcpu.arch.cr2;
	} else if (nr == DB_VECTOR) {
		/* See inject_pending_event.  */
		kvm_deliver_exception_payload(&svm->vcpu);
		if (svm->vcpu.arch.dr7 & DR7_GD) {
			svm->vcpu.arch.dr7 &= ~DR7_GD;
			kvm_update_dr7(&svm->vcpu);
		}
	} else
		WARN_ON(svm->vcpu.arch.exception.has_payload);

	nested_svm_vmexit(svm);
}