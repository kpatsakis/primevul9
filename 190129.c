void kvm_arch_flush_remote_tlbs_memslot(struct kvm *kvm,
					const struct kvm_memory_slot *memslot)
{
	/*
	 * All current use cases for flushing the TLBs for a specific memslot
	 * related to dirty logging, and many do the TLB flush out of mmu_lock.
	 * The interaction between the various operations on memslot must be
	 * serialized by slots_locks to ensure the TLB flush from one operation
	 * is observed by any other operation on the same memslot.
	 */
	lockdep_assert_held(&kvm->slots_lock);
	kvm_flush_remote_tlbs_with_address(kvm, memslot->base_gfn,
					   memslot->npages);
}