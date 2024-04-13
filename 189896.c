void kvm_mmu_unload(struct kvm_vcpu *vcpu)
{
	struct kvm *kvm = vcpu->kvm;

	kvm_mmu_free_roots(kvm, &vcpu->arch.root_mmu, KVM_MMU_ROOTS_ALL);
	WARN_ON(VALID_PAGE(vcpu->arch.root_mmu.root.hpa));
	kvm_mmu_free_roots(kvm, &vcpu->arch.guest_mmu, KVM_MMU_ROOTS_ALL);
	WARN_ON(VALID_PAGE(vcpu->arch.guest_mmu.root.hpa));
	vcpu_clear_mmio_info(vcpu, MMIO_GVA_ANY);
}