static void kvm_zap_obsolete_pages(struct kvm *kvm)
{
	struct kvm_mmu_page *sp, *node;
	int nr_zapped, batch = 0;
	bool unstable;

restart:
	list_for_each_entry_safe_reverse(sp, node,
	      &kvm->arch.active_mmu_pages, link) {
		/*
		 * No obsolete valid page exists before a newly created page
		 * since active_mmu_pages is a FIFO list.
		 */
		if (!is_obsolete_sp(kvm, sp))
			break;

		/*
		 * Invalid pages should never land back on the list of active
		 * pages.  Skip the bogus page, otherwise we'll get stuck in an
		 * infinite loop if the page gets put back on the list (again).
		 */
		if (WARN_ON(sp->role.invalid))
			continue;

		/*
		 * No need to flush the TLB since we're only zapping shadow
		 * pages with an obsolete generation number and all vCPUS have
		 * loaded a new root, i.e. the shadow pages being zapped cannot
		 * be in active use by the guest.
		 */
		if (batch >= BATCH_ZAP_PAGES &&
		    cond_resched_rwlock_write(&kvm->mmu_lock)) {
			batch = 0;
			goto restart;
		}

		unstable = __kvm_mmu_prepare_zap_page(kvm, sp,
				&kvm->arch.zapped_obsolete_pages, &nr_zapped);
		batch += nr_zapped;

		if (unstable)
			goto restart;
	}

	/*
	 * Kick all vCPUs (via remote TLB flush) before freeing the page tables
	 * to ensure KVM is not in the middle of a lockless shadow page table
	 * walk, which may reference the pages.  The remote TLB flush itself is
	 * not required and is simply a convenient way to kick vCPUs as needed.
	 * KVM performs a local TLB flush when allocating a new root (see
	 * kvm_mmu_load()), and the reload in the caller ensure no vCPUs are
	 * running with an obsolete MMU.
	 */
	kvm_mmu_commit_zap_page(kvm, &kvm->arch.zapped_obsolete_pages);
}