static int __kvm_mmu_create(struct kvm_vcpu *vcpu, struct kvm_mmu *mmu)
{
	struct page *page;
	int i;

	mmu->root.hpa = INVALID_PAGE;
	mmu->root.pgd = 0;
	for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++)
		mmu->prev_roots[i] = KVM_MMU_ROOT_INFO_INVALID;

	/* vcpu->arch.guest_mmu isn't used when !tdp_enabled. */
	if (!tdp_enabled && mmu == &vcpu->arch.guest_mmu)
		return 0;

	/*
	 * When using PAE paging, the four PDPTEs are treated as 'root' pages,
	 * while the PDP table is a per-vCPU construct that's allocated at MMU
	 * creation.  When emulating 32-bit mode, cr3 is only 32 bits even on
	 * x86_64.  Therefore we need to allocate the PDP table in the first
	 * 4GB of memory, which happens to fit the DMA32 zone.  TDP paging
	 * generally doesn't use PAE paging and can skip allocating the PDP
	 * table.  The main exception, handled here, is SVM's 32-bit NPT.  The
	 * other exception is for shadowing L1's 32-bit or PAE NPT on 64-bit
	 * KVM; that horror is handled on-demand by mmu_alloc_special_roots().
	 */
	if (tdp_enabled && kvm_mmu_get_tdp_level(vcpu) > PT32E_ROOT_LEVEL)
		return 0;

	page = alloc_page(GFP_KERNEL_ACCOUNT | __GFP_DMA32);
	if (!page)
		return -ENOMEM;

	mmu->pae_root = page_address(page);

	/*
	 * CR3 is only 32 bits when PAE paging is used, thus it's impossible to
	 * get the CPU to treat the PDPTEs as encrypted.  Decrypt the page so
	 * that KVM's writes and the CPU's reads get along.  Note, this is
	 * only necessary when using shadow paging, as 64-bit NPT can get at
	 * the C-bit even when shadowing 32-bit NPT, and SME isn't supported
	 * by 32-bit kernels (when KVM itself uses 32-bit NPT).
	 */
	if (!tdp_enabled)
		set_memory_decrypted((unsigned long)mmu->pae_root, 1);
	else
		WARN_ON_ONCE(shadow_me_mask);

	for (i = 0; i < 4; ++i)
		mmu->pae_root[i] = INVALID_PAE_ROOT;

	return 0;
}