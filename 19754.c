void sev_free_vcpu(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm;

	if (!sev_es_guest(vcpu->kvm))
		return;

	svm = to_svm(vcpu);

	if (vcpu->arch.guest_state_protected)
		sev_flush_guest_memory(svm, svm->vmsa, PAGE_SIZE);
	__free_page(virt_to_page(svm->vmsa));

	if (svm->ghcb_sa_free)
		kfree(svm->ghcb_sa);
}