void svm_leave_nested(struct vcpu_svm *svm)
{
	struct kvm_vcpu *vcpu = &svm->vcpu;

	if (is_guest_mode(vcpu)) {
		svm->nested.nested_run_pending = 0;
		svm->nested.vmcb12_gpa = INVALID_GPA;

		leave_guest_mode(vcpu);

		svm_switch_vmcb(svm, &svm->vmcb01);

		nested_svm_uninit_mmu_context(vcpu);
		vmcb_mark_all_dirty(svm->vmcb);
	}

	kvm_clear_request(KVM_REQ_GET_NESTED_STATE_PAGES, vcpu);
}