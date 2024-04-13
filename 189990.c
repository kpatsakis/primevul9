static int mmu_unsync_walk(struct kvm_mmu_page *sp,
			   struct kvm_mmu_pages *pvec)
{
	pvec->nr = 0;
	if (!sp->unsync_children)
		return 0;

	mmu_pages_add(pvec, sp, INVALID_INDEX);
	return __mmu_unsync_walk(sp, pvec);
}