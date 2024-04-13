void kvm_mmu_zap_all(struct kvm *kvm)
{
	struct kvm_mmu_page *sp, *node;
	LIST_HEAD(invalid_list);
	int ign;

	write_lock(&kvm->mmu_lock);
restart:
	list_for_each_entry_safe(sp, node, &kvm->arch.active_mmu_pages, link) {
		if (WARN_ON(sp->role.invalid))
			continue;
		if (__kvm_mmu_prepare_zap_page(kvm, sp, &invalid_list, &ign))
			goto restart;
		if (cond_resched_rwlock_write(&kvm->mmu_lock))
			goto restart;
	}

	kvm_mmu_commit_zap_page(kvm, &invalid_list);

	if (is_tdp_mmu_enabled(kvm))
		kvm_tdp_mmu_zap_all(kvm);

	write_unlock(&kvm->mmu_lock);
}