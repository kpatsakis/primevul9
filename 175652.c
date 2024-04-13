static void kvmgt_page_track_flush_slot(struct kvm *kvm,
		struct kvm_memory_slot *slot,
		struct kvm_page_track_notifier_node *node)
{
	int i;
	gfn_t gfn;
	struct kvmgt_guest_info *info = container_of(node,
					struct kvmgt_guest_info, track_node);

	spin_lock(&kvm->mmu_lock);
	for (i = 0; i < slot->npages; i++) {
		gfn = slot->base_gfn + i;
		if (kvmgt_gfn_is_write_protected(info, gfn)) {
			kvm_slot_page_track_remove_page(kvm, slot, gfn,
						KVM_PAGE_TRACK_WRITE);
			kvmgt_protect_table_del(info, gfn);
		}
	}
	spin_unlock(&kvm->mmu_lock);
}