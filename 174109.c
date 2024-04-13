
void kvm_inc_notifier_count(struct kvm *kvm, unsigned long start,
				   unsigned long end)
{
	/*
	 * The count increase must become visible at unlock time as no
	 * spte can be established without taking the mmu_lock and
	 * count is also read inside the mmu_lock critical section.
	 */
	kvm->mmu_notifier_count++;
	if (likely(kvm->mmu_notifier_count == 1)) {
		kvm->mmu_notifier_range_start = start;
		kvm->mmu_notifier_range_end = end;
	} else {
		/*
		 * Fully tracking multiple concurrent ranges has dimishing
		 * returns. Keep things simple and just find the minimal range
		 * which includes the current and new ranges. As there won't be
		 * enough information to subtract a range after its invalidate
		 * completes, any ranges invalidated concurrently will
		 * accumulate and persist until all outstanding invalidates
		 * complete.
		 */
		kvm->mmu_notifier_range_start =
			min(kvm->mmu_notifier_range_start, start);
		kvm->mmu_notifier_range_end =
			max(kvm->mmu_notifier_range_end, end);
	}