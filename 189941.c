int kvm_mmu_create(struct kvm_vcpu *vcpu)
{
	int ret;

	vcpu->arch.mmu_pte_list_desc_cache.kmem_cache = pte_list_desc_cache;
	vcpu->arch.mmu_pte_list_desc_cache.gfp_zero = __GFP_ZERO;

	vcpu->arch.mmu_page_header_cache.kmem_cache = mmu_page_header_cache;
	vcpu->arch.mmu_page_header_cache.gfp_zero = __GFP_ZERO;

	vcpu->arch.mmu_shadow_page_cache.gfp_zero = __GFP_ZERO;

	vcpu->arch.mmu = &vcpu->arch.root_mmu;
	vcpu->arch.walk_mmu = &vcpu->arch.root_mmu;

	ret = __kvm_mmu_create(vcpu, &vcpu->arch.guest_mmu);
	if (ret)
		return ret;

	ret = __kvm_mmu_create(vcpu, &vcpu->arch.root_mmu);
	if (ret)
		goto fail_allocate_root;

	return ret;
 fail_allocate_root:
	free_mmu_pages(&vcpu->arch.guest_mmu);
	return ret;
}