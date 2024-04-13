void __oom_reap_task_mm(struct mm_struct *mm)
{
	struct vm_area_struct *vma;

	/*
	 * Tell all users of get_user/copy_from_user etc... that the content
	 * is no longer stable. No barriers really needed because unmapping
	 * should imply barriers already and the reader would hit a page fault
	 * if it stumbled over a reaped memory.
	 */
	set_bit(MMF_UNSTABLE, &mm->flags);

	for (vma = mm->mmap ; vma; vma = vma->vm_next) {
		if (!can_madv_dontneed_vma(vma))
			continue;

		/*
		 * Only anonymous pages have a good chance to be dropped
		 * without additional steps which we cannot afford as we
		 * are OOM already.
		 *
		 * We do not even care about fs backed pages because all
		 * which are reclaimable have already been reclaimed and
		 * we do not want to block exit_mmap by keeping mm ref
		 * count elevated without a good reason.
		 */
		if (vma_is_anonymous(vma) || !(vma->vm_flags & VM_SHARED)) {
			const unsigned long start = vma->vm_start;
			const unsigned long end = vma->vm_end;
			struct mmu_gather tlb;

			tlb_gather_mmu(&tlb, mm, start, end);
			mmu_notifier_invalidate_range_start(mm, start, end);
			unmap_page_range(&tlb, vma, start, end, NULL);
			mmu_notifier_invalidate_range_end(mm, start, end);
			tlb_finish_mmu(&tlb, start, end);
		}
	}
}