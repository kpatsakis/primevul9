void kvm_mmu_free_obsolete_roots(struct kvm_vcpu *vcpu)
{
	__kvm_mmu_free_obsolete_roots(vcpu->kvm, &vcpu->arch.root_mmu);
	__kvm_mmu_free_obsolete_roots(vcpu->kvm, &vcpu->arch.guest_mmu);
}