static struct pte_list_desc *mmu_alloc_pte_list_desc(struct kvm_vcpu *vcpu)
{
	return kvm_mmu_memory_cache_alloc(&vcpu->arch.mmu_pte_list_desc_cache);
}