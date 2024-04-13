static void kvm_mmu_pte_write(struct kvm_vcpu *vcpu, gpa_t gpa,
			      const u8 *new, int bytes,
			      struct kvm_page_track_notifier_node *node)
{
	gfn_t gfn = gpa >> PAGE_SHIFT;
	struct kvm_mmu_page *sp;
	LIST_HEAD(invalid_list);
	u64 entry, gentry, *spte;
	int npte;
	bool flush = false;

	/*
	 * If we don't have indirect shadow pages, it means no page is
	 * write-protected, so we can exit simply.
	 */
	if (!READ_ONCE(vcpu->kvm->arch.indirect_shadow_pages))
		return;

	pgprintk("%s: gpa %llx bytes %d\n", __func__, gpa, bytes);

	/*
	 * No need to care whether allocation memory is successful
	 * or not since pte prefetch is skipped if it does not have
	 * enough objects in the cache.
	 */
	mmu_topup_memory_caches(vcpu, true);

	write_lock(&vcpu->kvm->mmu_lock);

	gentry = mmu_pte_write_fetch_gpte(vcpu, &gpa, &bytes);

	++vcpu->kvm->stat.mmu_pte_write;

	for_each_gfn_indirect_valid_sp(vcpu->kvm, sp, gfn) {
		if (detect_write_misaligned(sp, gpa, bytes) ||
		      detect_write_flooding(sp)) {
			kvm_mmu_prepare_zap_page(vcpu->kvm, sp, &invalid_list);
			++vcpu->kvm->stat.mmu_flooded;
			continue;
		}

		spte = get_written_sptes(sp, gpa, &npte);
		if (!spte)
			continue;

		while (npte--) {
			entry = *spte;
			mmu_page_zap_pte(vcpu->kvm, sp, spte, NULL);
			if (gentry && sp->role.level != PG_LEVEL_4K)
				++vcpu->kvm->stat.mmu_pde_zapped;
			if (need_remote_flush(entry, *spte))
				flush = true;
			++spte;
		}
	}
	kvm_mmu_remote_flush_or_zap(vcpu->kvm, &invalid_list, flush);
	write_unlock(&vcpu->kvm->mmu_lock);
}