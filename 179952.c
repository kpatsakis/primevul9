static int kvm_mmu_notifier_invalidate_range_start(struct mmu_notifier *mn,
					const struct mmu_notifier_range *range)
{
	struct kvm *kvm = mmu_notifier_to_kvm(mn);
	int need_tlb_flush = 0, idx;

	idx = srcu_read_lock(&kvm->srcu);
	spin_lock(&kvm->mmu_lock);
	/*
	 * The count increase must become visible at unlock time as no
	 * spte can be established without taking the mmu_lock and
	 * count is also read inside the mmu_lock critical section.
	 */
	kvm->mmu_notifier_count++;
	need_tlb_flush = kvm_unmap_hva_range(kvm, range->start, range->end,
					     range->flags);
	need_tlb_flush |= kvm->tlbs_dirty;
	/* we've to flush the tlb before the pages can be freed */
	if (need_tlb_flush)
		kvm_flush_remote_tlbs(kvm);

	spin_unlock(&kvm->mmu_lock);
	srcu_read_unlock(&kvm->srcu, idx);

	return 0;
}