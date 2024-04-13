static void kvm_mmu_invalidate_zap_pages_in_memslot(struct kvm *kvm,
			struct kvm_memory_slot *slot,
			struct kvm_page_track_notifier_node *node)
{
	kvm_mmu_zap_all_fast(kvm);
}