static void mmu_free_memory_caches(struct kvm_vcpu *vcpu)
{
	kvm_mmu_free_memory_cache(&vcpu->arch.mmu_pte_list_desc_cache);
	kvm_mmu_free_memory_cache(&vcpu->arch.mmu_shadow_page_cache);
	kvm_mmu_free_memory_cache(&vcpu->arch.mmu_gfn_array_cache);
	kvm_mmu_free_memory_cache(&vcpu->arch.mmu_page_header_cache);
}