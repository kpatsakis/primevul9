static bool page_fault_can_be_fast(struct kvm_page_fault *fault)
{
	/*
	 * Do not fix the mmio spte with invalid generation number which
	 * need to be updated by slow page fault path.
	 */
	if (fault->rsvd)
		return false;

	/* See if the page fault is due to an NX violation */
	if (unlikely(fault->exec && fault->present))
		return false;

	/*
	 * #PF can be fast if:
	 * 1. The shadow page table entry is not present, which could mean that
	 *    the fault is potentially caused by access tracking (if enabled).
	 * 2. The shadow page table entry is present and the fault
	 *    is caused by write-protect, that means we just need change the W
	 *    bit of the spte which can be done out of mmu-lock.
	 *
	 * However, if access tracking is disabled we know that a non-present
	 * page must be a genuine page fault where we have to create a new SPTE.
	 * So, if access tracking is disabled, we return true only for write
	 * accesses to a present page.
	 */

	return shadow_acc_track_mask != 0 || (fault->write && fault->present);
}