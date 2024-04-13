static void kvm_recover_nx_lpages(struct kvm *kvm)
{
	unsigned long nx_lpage_splits = kvm->stat.nx_lpage_splits;
	int rcu_idx;
	struct kvm_mmu_page *sp;
	unsigned int ratio;
	LIST_HEAD(invalid_list);
	bool flush = false;
	ulong to_zap;

	rcu_idx = srcu_read_lock(&kvm->srcu);
	write_lock(&kvm->mmu_lock);

	/*
	 * Zapping TDP MMU shadow pages, including the remote TLB flush, must
	 * be done under RCU protection, because the pages are freed via RCU
	 * callback.
	 */
	rcu_read_lock();

	ratio = READ_ONCE(nx_huge_pages_recovery_ratio);
	to_zap = ratio ? DIV_ROUND_UP(nx_lpage_splits, ratio) : 0;
	for ( ; to_zap; --to_zap) {
		if (list_empty(&kvm->arch.lpage_disallowed_mmu_pages))
			break;

		/*
		 * We use a separate list instead of just using active_mmu_pages
		 * because the number of lpage_disallowed pages is expected to
		 * be relatively small compared to the total.
		 */
		sp = list_first_entry(&kvm->arch.lpage_disallowed_mmu_pages,
				      struct kvm_mmu_page,
				      lpage_disallowed_link);
		WARN_ON_ONCE(!sp->lpage_disallowed);
		if (is_tdp_mmu_page(sp)) {
			flush |= kvm_tdp_mmu_zap_sp(kvm, sp);
		} else {
			kvm_mmu_prepare_zap_page(kvm, sp, &invalid_list);
			WARN_ON_ONCE(sp->lpage_disallowed);
		}

		if (need_resched() || rwlock_needbreak(&kvm->mmu_lock)) {
			kvm_mmu_remote_flush_or_zap(kvm, &invalid_list, flush);
			rcu_read_unlock();

			cond_resched_rwlock_write(&kvm->mmu_lock);
			flush = false;

			rcu_read_lock();
		}
	}
	kvm_mmu_remote_flush_or_zap(kvm, &invalid_list, flush);

	rcu_read_unlock();

	write_unlock(&kvm->mmu_lock);
	srcu_read_unlock(&kvm->srcu, rcu_idx);
}