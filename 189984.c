static int mmu_sync_children(struct kvm_vcpu *vcpu,
			     struct kvm_mmu_page *parent, bool can_yield)
{
	int i;
	struct kvm_mmu_page *sp;
	struct mmu_page_path parents;
	struct kvm_mmu_pages pages;
	LIST_HEAD(invalid_list);
	bool flush = false;

	while (mmu_unsync_walk(parent, &pages)) {
		bool protected = false;

		for_each_sp(pages, sp, parents, i)
			protected |= kvm_vcpu_write_protect_gfn(vcpu, sp->gfn);

		if (protected) {
			kvm_mmu_remote_flush_or_zap(vcpu->kvm, &invalid_list, true);
			flush = false;
		}

		for_each_sp(pages, sp, parents, i) {
			kvm_unlink_unsync_page(vcpu->kvm, sp);
			flush |= kvm_sync_page(vcpu, sp, &invalid_list);
			mmu_pages_clear_parents(&parents);
		}
		if (need_resched() || rwlock_needbreak(&vcpu->kvm->mmu_lock)) {
			kvm_mmu_remote_flush_or_zap(vcpu->kvm, &invalid_list, flush);
			if (!can_yield) {
				kvm_make_request(KVM_REQ_MMU_SYNC, vcpu);
				return -EINTR;
			}

			cond_resched_rwlock_write(&vcpu->kvm->mmu_lock);
			flush = false;
		}
	}

	kvm_mmu_remote_flush_or_zap(vcpu->kvm, &invalid_list, flush);
	return 0;
}