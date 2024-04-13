static int mmu_alloc_direct_roots(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *mmu = vcpu->arch.mmu;
	u8 shadow_root_level = mmu->shadow_root_level;
	hpa_t root;
	unsigned i;
	int r;

	write_lock(&vcpu->kvm->mmu_lock);
	r = make_mmu_pages_available(vcpu);
	if (r < 0)
		goto out_unlock;

	if (is_tdp_mmu_enabled(vcpu->kvm)) {
		root = kvm_tdp_mmu_get_vcpu_root_hpa(vcpu);
		mmu->root.hpa = root;
	} else if (shadow_root_level >= PT64_ROOT_4LEVEL) {
		root = mmu_alloc_root(vcpu, 0, 0, shadow_root_level, true);
		mmu->root.hpa = root;
	} else if (shadow_root_level == PT32E_ROOT_LEVEL) {
		if (WARN_ON_ONCE(!mmu->pae_root)) {
			r = -EIO;
			goto out_unlock;
		}

		for (i = 0; i < 4; ++i) {
			WARN_ON_ONCE(IS_VALID_PAE_ROOT(mmu->pae_root[i]));

			root = mmu_alloc_root(vcpu, i << (30 - PAGE_SHIFT),
					      i << 30, PT32_ROOT_LEVEL, true);
			mmu->pae_root[i] = root | PT_PRESENT_MASK |
					   shadow_me_mask;
		}
		mmu->root.hpa = __pa(mmu->pae_root);
	} else {
		WARN_ONCE(1, "Bad TDP root level = %d\n", shadow_root_level);
		r = -EIO;
		goto out_unlock;
	}

	/* root.pgd is ignored for direct MMUs. */
	mmu->root.pgd = 0;
out_unlock:
	write_unlock(&vcpu->kvm->mmu_lock);
	return r;
}