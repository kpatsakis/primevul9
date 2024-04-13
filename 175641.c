static int kvmgt_page_track_remove(unsigned long handle, u64 gfn)
{
	struct kvmgt_guest_info *info;
	struct kvm *kvm;
	struct kvm_memory_slot *slot;
	int idx;

	if (!handle_valid(handle))
		return 0;

	info = (struct kvmgt_guest_info *)handle;
	kvm = info->kvm;

	idx = srcu_read_lock(&kvm->srcu);
	slot = gfn_to_memslot(kvm, gfn);
	if (!slot) {
		srcu_read_unlock(&kvm->srcu, idx);
		return -EINVAL;
	}

	spin_lock(&kvm->mmu_lock);

	if (!kvmgt_gfn_is_write_protected(info, gfn))
		goto out;

	kvm_slot_page_track_remove_page(kvm, slot, gfn, KVM_PAGE_TRACK_WRITE);
	kvmgt_protect_table_del(info, gfn);

out:
	spin_unlock(&kvm->mmu_lock);
	srcu_read_unlock(&kvm->srcu, idx);
	return 0;
}