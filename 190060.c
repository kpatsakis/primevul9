static int mmu_first_shadow_root_alloc(struct kvm *kvm)
{
	struct kvm_memslots *slots;
	struct kvm_memory_slot *slot;
	int r = 0, i, bkt;

	/*
	 * Check if this is the first shadow root being allocated before
	 * taking the lock.
	 */
	if (kvm_shadow_root_allocated(kvm))
		return 0;

	mutex_lock(&kvm->slots_arch_lock);

	/* Recheck, under the lock, whether this is the first shadow root. */
	if (kvm_shadow_root_allocated(kvm))
		goto out_unlock;

	/*
	 * Check if anything actually needs to be allocated, e.g. all metadata
	 * will be allocated upfront if TDP is disabled.
	 */
	if (kvm_memslots_have_rmaps(kvm) &&
	    kvm_page_track_write_tracking_enabled(kvm))
		goto out_success;

	for (i = 0; i < KVM_ADDRESS_SPACE_NUM; i++) {
		slots = __kvm_memslots(kvm, i);
		kvm_for_each_memslot(slot, bkt, slots) {
			/*
			 * Both of these functions are no-ops if the target is
			 * already allocated, so unconditionally calling both
			 * is safe.  Intentionally do NOT free allocations on
			 * failure to avoid having to track which allocations
			 * were made now versus when the memslot was created.
			 * The metadata is guaranteed to be freed when the slot
			 * is freed, and will be kept/used if userspace retries
			 * KVM_RUN instead of killing the VM.
			 */
			r = memslot_rmap_alloc(slot, slot->npages);
			if (r)
				goto out_unlock;
			r = kvm_page_track_write_tracking_alloc(slot);
			if (r)
				goto out_unlock;
		}
	}

	/*
	 * Ensure that shadow_root_allocated becomes true strictly after
	 * all the related pointers are set.
	 */
out_success:
	smp_store_release(&kvm->arch.shadow_root_allocated, true);

out_unlock:
	mutex_unlock(&kvm->slots_arch_lock);
	return r;
}