int mmu_try_to_unsync_pages(struct kvm *kvm, const struct kvm_memory_slot *slot,
			    gfn_t gfn, bool can_unsync, bool prefetch)
{
	struct kvm_mmu_page *sp;
	bool locked = false;

	/*
	 * Force write-protection if the page is being tracked.  Note, the page
	 * track machinery is used to write-protect upper-level shadow pages,
	 * i.e. this guards the role.level == 4K assertion below!
	 */
	if (kvm_slot_page_track_is_active(kvm, slot, gfn, KVM_PAGE_TRACK_WRITE))
		return -EPERM;

	/*
	 * The page is not write-tracked, mark existing shadow pages unsync
	 * unless KVM is synchronizing an unsync SP (can_unsync = false).  In
	 * that case, KVM must complete emulation of the guest TLB flush before
	 * allowing shadow pages to become unsync (writable by the guest).
	 */
	for_each_gfn_indirect_valid_sp(kvm, sp, gfn) {
		if (!can_unsync)
			return -EPERM;

		if (sp->unsync)
			continue;

		if (prefetch)
			return -EEXIST;

		/*
		 * TDP MMU page faults require an additional spinlock as they
		 * run with mmu_lock held for read, not write, and the unsync
		 * logic is not thread safe.  Take the spinklock regardless of
		 * the MMU type to avoid extra conditionals/parameters, there's
		 * no meaningful penalty if mmu_lock is held for write.
		 */
		if (!locked) {
			locked = true;
			spin_lock(&kvm->arch.mmu_unsync_pages_lock);

			/*
			 * Recheck after taking the spinlock, a different vCPU
			 * may have since marked the page unsync.  A false
			 * positive on the unprotected check above is not
			 * possible as clearing sp->unsync _must_ hold mmu_lock
			 * for write, i.e. unsync cannot transition from 0->1
			 * while this CPU holds mmu_lock for read (or write).
			 */
			if (READ_ONCE(sp->unsync))
				continue;
		}

		WARN_ON(sp->role.level != PG_LEVEL_4K);
		kvm_unsync_page(kvm, sp);
	}
	if (locked)
		spin_unlock(&kvm->arch.mmu_unsync_pages_lock);

	/*
	 * We need to ensure that the marking of unsync pages is visible
	 * before the SPTE is updated to allow writes because
	 * kvm_mmu_sync_roots() checks the unsync flags without holding
	 * the MMU lock and so can race with this. If the SPTE was updated
	 * before the page had been marked as unsync-ed, something like the
	 * following could happen:
	 *
	 * CPU 1                    CPU 2
	 * ---------------------------------------------------------------------
	 * 1.2 Host updates SPTE
	 *     to be writable
	 *                      2.1 Guest writes a GPTE for GVA X.
	 *                          (GPTE being in the guest page table shadowed
	 *                           by the SP from CPU 1.)
	 *                          This reads SPTE during the page table walk.
	 *                          Since SPTE.W is read as 1, there is no
	 *                          fault.
	 *
	 *                      2.2 Guest issues TLB flush.
	 *                          That causes a VM Exit.
	 *
	 *                      2.3 Walking of unsync pages sees sp->unsync is
	 *                          false and skips the page.
	 *
	 *                      2.4 Guest accesses GVA X.
	 *                          Since the mapping in the SP was not updated,
	 *                          so the old mapping for GVA X incorrectly
	 *                          gets used.
	 * 1.1 Host marks SP
	 *     as unsync
	 *     (sp->unsync = true)
	 *
	 * The write barrier below ensures that 1.1 happens before 1.2 and thus
	 * the situation in 2.4 does not arise.  It pairs with the read barrier
	 * in is_unsync_root(), placed between 2.1's load of SPTE.W and 2.3.
	 */
	smp_wmb();

	return 0;
}