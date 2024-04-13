void kvm_mmu_destroy(struct kvm_vcpu *vcpu)
{
	kvm_mmu_unload(vcpu);
	free_mmu_pages(&vcpu->arch.root_mmu);
	free_mmu_pages(&vcpu->arch.guest_mmu);
	mmu_free_memory_caches(vcpu);
}