static int mmu_alloc_special_roots(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *mmu = vcpu->arch.mmu;
	bool need_pml5 = mmu->shadow_root_level > PT64_ROOT_4LEVEL;
	u64 *pml5_root = NULL;
	u64 *pml4_root = NULL;
	u64 *pae_root;

	/*
	 * When shadowing 32-bit or PAE NPT with 64-bit NPT, the PML4 and PDP
	 * tables are allocated and initialized at root creation as there is no
	 * equivalent level in the guest's NPT to shadow.  Allocate the tables
	 * on demand, as running a 32-bit L1 VMM on 64-bit KVM is very rare.
	 */
	if (mmu->direct_map || mmu->root_level >= PT64_ROOT_4LEVEL ||
	    mmu->shadow_root_level < PT64_ROOT_4LEVEL)
		return 0;

	/*
	 * NPT, the only paging mode that uses this horror, uses a fixed number
	 * of levels for the shadow page tables, e.g. all MMUs are 4-level or
	 * all MMus are 5-level.  Thus, this can safely require that pml5_root
	 * is allocated if the other roots are valid and pml5 is needed, as any
	 * prior MMU would also have required pml5.
	 */
	if (mmu->pae_root && mmu->pml4_root && (!need_pml5 || mmu->pml5_root))
		return 0;

	/*
	 * The special roots should always be allocated in concert.  Yell and
	 * bail if KVM ends up in a state where only one of the roots is valid.
	 */
	if (WARN_ON_ONCE(!tdp_enabled || mmu->pae_root || mmu->pml4_root ||
			 (need_pml5 && mmu->pml5_root)))
		return -EIO;

	/*
	 * Unlike 32-bit NPT, the PDP table doesn't need to be in low mem, and
	 * doesn't need to be decrypted.
	 */
	pae_root = (void *)get_zeroed_page(GFP_KERNEL_ACCOUNT);
	if (!pae_root)
		return -ENOMEM;

#ifdef CONFIG_X86_64
	pml4_root = (void *)get_zeroed_page(GFP_KERNEL_ACCOUNT);
	if (!pml4_root)
		goto err_pml4;

	if (need_pml5) {
		pml5_root = (void *)get_zeroed_page(GFP_KERNEL_ACCOUNT);
		if (!pml5_root)
			goto err_pml5;
	}
#endif

	mmu->pae_root = pae_root;
	mmu->pml4_root = pml4_root;
	mmu->pml5_root = pml5_root;

	return 0;

#ifdef CONFIG_X86_64
err_pml5:
	free_page((unsigned long)pml4_root);
err_pml4:
	free_page((unsigned long)pae_root);
	return -ENOMEM;
#endif
}