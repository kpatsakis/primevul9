int nested_svm_check_permissions(struct kvm_vcpu *vcpu)
{
	if (!(vcpu->arch.efer & EFER_SVME) || !is_paging(vcpu)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	if (to_svm(vcpu)->vmcb->save.cpl) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	}

	return 0;
}