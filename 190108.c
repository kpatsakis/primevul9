static struct kvm_mmu_page *kvm_mmu_get_page(struct kvm_vcpu *vcpu,
					     gfn_t gfn,
					     gva_t gaddr,
					     unsigned level,
					     int direct,
					     unsigned int access)
{
	bool direct_mmu = vcpu->arch.mmu->direct_map;
	union kvm_mmu_page_role role;
	struct hlist_head *sp_list;
	unsigned quadrant;
	struct kvm_mmu_page *sp;
	int collisions = 0;
	LIST_HEAD(invalid_list);

	role = vcpu->arch.mmu->mmu_role.base;
	role.level = level;
	role.direct = direct;
	role.access = access;
	if (role.has_4_byte_gpte) {
		quadrant = gaddr >> (PAGE_SHIFT + (PT64_PT_BITS * level));
		quadrant &= (1 << ((PT32_PT_BITS - PT64_PT_BITS) * level)) - 1;
		role.quadrant = quadrant;
	}

	sp_list = &vcpu->kvm->arch.mmu_page_hash[kvm_page_table_hashfn(gfn)];
	for_each_valid_sp(vcpu->kvm, sp, sp_list) {
		if (sp->gfn != gfn) {
			collisions++;
			continue;
		}

		if (sp->role.word != role.word) {
			/*
			 * If the guest is creating an upper-level page, zap
			 * unsync pages for the same gfn.  While it's possible
			 * the guest is using recursive page tables, in all
			 * likelihood the guest has stopped using the unsync
			 * page and is installing a completely unrelated page.
			 * Unsync pages must not be left as is, because the new
			 * upper-level page will be write-protected.
			 */
			if (level > PG_LEVEL_4K && sp->unsync)
				kvm_mmu_prepare_zap_page(vcpu->kvm, sp,
							 &invalid_list);
			continue;
		}

		if (direct_mmu)
			goto trace_get_page;

		if (sp->unsync) {
			/*
			 * The page is good, but is stale.  kvm_sync_page does
			 * get the latest guest state, but (unlike mmu_unsync_children)
			 * it doesn't write-protect the page or mark it synchronized!
			 * This way the validity of the mapping is ensured, but the
			 * overhead of write protection is not incurred until the
			 * guest invalidates the TLB mapping.  This allows multiple
			 * SPs for a single gfn to be unsync.
			 *
			 * If the sync fails, the page is zapped.  If so, break
			 * in order to rebuild it.
			 */
			if (!kvm_sync_page(vcpu, sp, &invalid_list))
				break;

			WARN_ON(!list_empty(&invalid_list));
			kvm_flush_remote_tlbs(vcpu->kvm);
		}

		__clear_sp_write_flooding_count(sp);

trace_get_page:
		trace_kvm_mmu_get_page(sp, false);
		goto out;
	}

	++vcpu->kvm->stat.mmu_cache_miss;

	sp = kvm_mmu_alloc_page(vcpu, direct);

	sp->gfn = gfn;
	sp->role = role;
	hlist_add_head(&sp->hash_link, sp_list);
	if (!direct) {
		account_shadowed(vcpu->kvm, sp);
		if (level == PG_LEVEL_4K && kvm_vcpu_write_protect_gfn(vcpu, gfn))
			kvm_flush_remote_tlbs_with_address(vcpu->kvm, gfn, 1);
	}
	trace_kvm_mmu_get_page(sp, true);
out:
	kvm_mmu_commit_zap_page(vcpu->kvm, &invalid_list);

	if (collisions > vcpu->kvm->stat.max_mmu_page_hash_collisions)
		vcpu->kvm->stat.max_mmu_page_hash_collisions = collisions;
	return sp;
}