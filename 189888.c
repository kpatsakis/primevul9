static int mmu_alloc_shadow_roots(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *mmu = vcpu->arch.mmu;
	u64 pdptrs[4], pm_mask;
	gfn_t root_gfn, root_pgd;
	hpa_t root;
	unsigned i;
	int r;

	root_pgd = mmu->get_guest_pgd(vcpu);
	root_gfn = root_pgd >> PAGE_SHIFT;

	if (mmu_check_root(vcpu, root_gfn))
		return 1;

	/*
	 * On SVM, reading PDPTRs might access guest memory, which might fault
	 * and thus might sleep.  Grab the PDPTRs before acquiring mmu_lock.
	 */
	if (mmu->root_level == PT32E_ROOT_LEVEL) {
		for (i = 0; i < 4; ++i) {
			pdptrs[i] = mmu->get_pdptr(vcpu, i);
			if (!(pdptrs[i] & PT_PRESENT_MASK))
				continue;

			if (mmu_check_root(vcpu, pdptrs[i] >> PAGE_SHIFT))
				return 1;
		}
	}

	r = mmu_first_shadow_root_alloc(vcpu->kvm);
	if (r)
		return r;

	write_lock(&vcpu->kvm->mmu_lock);
	r = make_mmu_pages_available(vcpu);
	if (r < 0)
		goto out_unlock;

	/*
	 * Do we shadow a long mode page table? If so we need to
	 * write-protect the guests page table root.
	 */
	if (mmu->root_level >= PT64_ROOT_4LEVEL) {
		root = mmu_alloc_root(vcpu, root_gfn, 0,
				      mmu->shadow_root_level, false);
		mmu->root.hpa = root;
		goto set_root_pgd;
	}

	if (WARN_ON_ONCE(!mmu->pae_root)) {
		r = -EIO;
		goto out_unlock;
	}

	/*
	 * We shadow a 32 bit page table. This may be a legacy 2-level
	 * or a PAE 3-level page table. In either case we need to be aware that
	 * the shadow page table may be a PAE or a long mode page table.
	 */
	pm_mask = PT_PRESENT_MASK | shadow_me_mask;
	if (mmu->shadow_root_level >= PT64_ROOT_4LEVEL) {
		pm_mask |= PT_ACCESSED_MASK | PT_WRITABLE_MASK | PT_USER_MASK;

		if (WARN_ON_ONCE(!mmu->pml4_root)) {
			r = -EIO;
			goto out_unlock;
		}
		mmu->pml4_root[0] = __pa(mmu->pae_root) | pm_mask;

		if (mmu->shadow_root_level == PT64_ROOT_5LEVEL) {
			if (WARN_ON_ONCE(!mmu->pml5_root)) {
				r = -EIO;
				goto out_unlock;
			}
			mmu->pml5_root[0] = __pa(mmu->pml4_root) | pm_mask;
		}
	}

	for (i = 0; i < 4; ++i) {
		WARN_ON_ONCE(IS_VALID_PAE_ROOT(mmu->pae_root[i]));

		if (mmu->root_level == PT32E_ROOT_LEVEL) {
			if (!(pdptrs[i] & PT_PRESENT_MASK)) {
				mmu->pae_root[i] = INVALID_PAE_ROOT;
				continue;
			}
			root_gfn = pdptrs[i] >> PAGE_SHIFT;
		}

		root = mmu_alloc_root(vcpu, root_gfn, i << 30,
				      PT32_ROOT_LEVEL, false);
		mmu->pae_root[i] = root | pm_mask;
	}

	if (mmu->shadow_root_level == PT64_ROOT_5LEVEL)
		mmu->root.hpa = __pa(mmu->pml5_root);
	else if (mmu->shadow_root_level == PT64_ROOT_4LEVEL)
		mmu->root.hpa = __pa(mmu->pml4_root);
	else
		mmu->root.hpa = __pa(mmu->pae_root);

set_root_pgd:
	mmu->root.pgd = root_pgd;
out_unlock:
	write_unlock(&vcpu->kvm->mmu_lock);

	return r;
}