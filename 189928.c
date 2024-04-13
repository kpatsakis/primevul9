static bool is_unsync_root(hpa_t root)
{
	struct kvm_mmu_page *sp;

	if (!VALID_PAGE(root))
		return false;

	/*
	 * The read barrier orders the CPU's read of SPTE.W during the page table
	 * walk before the reads of sp->unsync/sp->unsync_children here.
	 *
	 * Even if another CPU was marking the SP as unsync-ed simultaneously,
	 * any guest page table changes are not guaranteed to be visible anyway
	 * until this VCPU issues a TLB flush strictly after those changes are
	 * made.  We only need to ensure that the other CPU sets these flags
	 * before any actual changes to the page tables are made.  The comments
	 * in mmu_try_to_unsync_pages() describe what could go wrong if this
	 * requirement isn't satisfied.
	 */
	smp_rmb();
	sp = to_shadow_page(root);

	/*
	 * PAE roots (somewhat arbitrarily) aren't backed by shadow pages, the
	 * PDPTEs for a given PAE root need to be synchronized individually.
	 */
	if (WARN_ON_ONCE(!sp))
		return false;

	if (sp->unsync || sp->unsync_children)
		return true;

	return false;
}