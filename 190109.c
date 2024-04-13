void kvm_mmu_zap_collapsible_sptes(struct kvm *kvm,
				   const struct kvm_memory_slot *slot)
{
	if (kvm_memslots_have_rmaps(kvm)) {
		write_lock(&kvm->mmu_lock);
		/*
		 * Zap only 4k SPTEs since the legacy MMU only supports dirty
		 * logging at a 4k granularity and never creates collapsible
		 * 2m SPTEs during dirty logging.
		 */
		if (slot_handle_level_4k(kvm, slot, kvm_mmu_zap_collapsible_spte, true))
			kvm_arch_flush_remote_tlbs_memslot(kvm, slot);
		write_unlock(&kvm->mmu_lock);
	}

	if (is_tdp_mmu_enabled(kvm)) {
		read_lock(&kvm->mmu_lock);
		kvm_tdp_mmu_zap_collapsible_sptes(kvm, slot);
		read_unlock(&kvm->mmu_lock);
	}
}