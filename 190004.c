static bool rmap_can_add(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu_memory_cache *mc;

	mc = &vcpu->arch.mmu_pte_list_desc_cache;
	return kvm_mmu_memory_cache_nr_free_objects(mc);
}