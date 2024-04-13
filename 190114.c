void kvm_mmu_sync_roots(struct kvm_vcpu *vcpu)
{
	int i;
	struct kvm_mmu_page *sp;

	if (vcpu->arch.mmu->direct_map)
		return;

	if (!VALID_PAGE(vcpu->arch.mmu->root.hpa))
		return;

	vcpu_clear_mmio_info(vcpu, MMIO_GVA_ANY);

	if (vcpu->arch.mmu->root_level >= PT64_ROOT_4LEVEL) {
		hpa_t root = vcpu->arch.mmu->root.hpa;
		sp = to_shadow_page(root);

		if (!is_unsync_root(root))
			return;

		write_lock(&vcpu->kvm->mmu_lock);
		mmu_sync_children(vcpu, sp, true);
		write_unlock(&vcpu->kvm->mmu_lock);
		return;
	}

	write_lock(&vcpu->kvm->mmu_lock);

	for (i = 0; i < 4; ++i) {
		hpa_t root = vcpu->arch.mmu->pae_root[i];

		if (IS_VALID_PAE_ROOT(root)) {
			root &= PT64_BASE_ADDR_MASK;
			sp = to_shadow_page(root);
			mmu_sync_children(vcpu, sp, true);
		}
	}

	write_unlock(&vcpu->kvm->mmu_lock);
}