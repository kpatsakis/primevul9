bool kvm_mmu_slot_gfn_write_protect(struct kvm *kvm,
				    struct kvm_memory_slot *slot, u64 gfn,
				    int min_level)
{
	struct kvm_rmap_head *rmap_head;
	int i;
	bool write_protected = false;

	if (kvm_memslots_have_rmaps(kvm)) {
		for (i = min_level; i <= KVM_MAX_HUGEPAGE_LEVEL; ++i) {
			rmap_head = gfn_to_rmap(gfn, i, slot);
			write_protected |= rmap_write_protect(rmap_head, true);
		}
	}

	if (is_tdp_mmu_enabled(kvm))
		write_protected |=
			kvm_tdp_mmu_write_protect_gfn(kvm, slot, gfn, min_level);

	return write_protected;
}