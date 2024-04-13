int kvm_mmu_init_vm(struct kvm *kvm)
{
	struct kvm_page_track_notifier_node *node = &kvm->arch.mmu_sp_tracker;
	int r;

	INIT_LIST_HEAD(&kvm->arch.active_mmu_pages);
	INIT_LIST_HEAD(&kvm->arch.zapped_obsolete_pages);
	INIT_LIST_HEAD(&kvm->arch.lpage_disallowed_mmu_pages);
	spin_lock_init(&kvm->arch.mmu_unsync_pages_lock);

	r = kvm_mmu_init_tdp_mmu(kvm);
	if (r < 0)
		return r;

	node->track_write = kvm_mmu_pte_write;
	node->track_flush_slot = kvm_mmu_invalidate_zap_pages_in_memslot;
	kvm_page_track_register_notifier(kvm, node);
	return 0;
}