static unsigned long kvm_mmu_zap_oldest_mmu_pages(struct kvm *kvm,
						  unsigned long nr_to_zap)
{
	unsigned long total_zapped = 0;
	struct kvm_mmu_page *sp, *tmp;
	LIST_HEAD(invalid_list);
	bool unstable;
	int nr_zapped;

	if (list_empty(&kvm->arch.active_mmu_pages))
		return 0;

restart:
	list_for_each_entry_safe_reverse(sp, tmp, &kvm->arch.active_mmu_pages, link) {
		/*
		 * Don't zap active root pages, the page itself can't be freed
		 * and zapping it will just force vCPUs to realloc and reload.
		 */
		if (sp->root_count)
			continue;

		unstable = __kvm_mmu_prepare_zap_page(kvm, sp, &invalid_list,
						      &nr_zapped);
		total_zapped += nr_zapped;
		if (total_zapped >= nr_to_zap)
			break;

		if (unstable)
			goto restart;
	}

	kvm_mmu_commit_zap_page(kvm, &invalid_list);

	kvm->stat.mmu_recycled += total_zapped;
	return total_zapped;
}