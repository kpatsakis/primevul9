static void svm_inject_page_fault_nested(struct kvm_vcpu *vcpu, struct x86_exception *fault)
{
       struct vcpu_svm *svm = to_svm(vcpu);
       WARN_ON(!is_guest_mode(vcpu));

       if (vmcb_is_intercept(&svm->nested.ctl, INTERCEPT_EXCEPTION_OFFSET + PF_VECTOR) &&
	   !svm->nested.nested_run_pending) {
               svm->vmcb->control.exit_code = SVM_EXIT_EXCP_BASE + PF_VECTOR;
               svm->vmcb->control.exit_code_hi = 0;
               svm->vmcb->control.exit_info_1 = fault->error_code;
               svm->vmcb->control.exit_info_2 = fault->address;
               nested_svm_vmexit(svm);
       } else {
               kvm_inject_page_fault(vcpu, fault);
       }
}