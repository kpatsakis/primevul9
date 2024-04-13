void kvm_mmu_reset_context(struct kvm_vcpu *vcpu)
{
	kvm_mmu_unload(vcpu);
	kvm_init_mmu(vcpu);
}