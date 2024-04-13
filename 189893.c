static bool kvm_faultin_pfn(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault, int *r)
{
	struct kvm_memory_slot *slot = fault->slot;
	bool async;

	/*
	 * Retry the page fault if the gfn hit a memslot that is being deleted
	 * or moved.  This ensures any existing SPTEs for the old memslot will
	 * be zapped before KVM inserts a new MMIO SPTE for the gfn.
	 */
	if (slot && (slot->flags & KVM_MEMSLOT_INVALID))
		goto out_retry;

	if (!kvm_is_visible_memslot(slot)) {
		/* Don't expose private memslots to L2. */
		if (is_guest_mode(vcpu)) {
			fault->slot = NULL;
			fault->pfn = KVM_PFN_NOSLOT;
			fault->map_writable = false;
			return false;
		}
		/*
		 * If the APIC access page exists but is disabled, go directly
		 * to emulation without caching the MMIO access or creating a
		 * MMIO SPTE.  That way the cache doesn't need to be purged
		 * when the AVIC is re-enabled.
		 */
		if (slot && slot->id == APIC_ACCESS_PAGE_PRIVATE_MEMSLOT &&
		    !kvm_apicv_activated(vcpu->kvm)) {
			*r = RET_PF_EMULATE;
			return true;
		}
	}

	async = false;
	fault->pfn = __gfn_to_pfn_memslot(slot, fault->gfn, false, &async,
					  fault->write, &fault->map_writable,
					  &fault->hva);
	if (!async)
		return false; /* *pfn has correct page already */

	if (!fault->prefetch && kvm_can_do_async_pf(vcpu)) {
		trace_kvm_try_async_get_page(fault->addr, fault->gfn);
		if (kvm_find_async_pf_gfn(vcpu, fault->gfn)) {
			trace_kvm_async_pf_doublefault(fault->addr, fault->gfn);
			kvm_make_request(KVM_REQ_APF_HALT, vcpu);
			goto out_retry;
		} else if (kvm_arch_setup_async_pf(vcpu, fault->addr, fault->gfn))
			goto out_retry;
	}

	fault->pfn = __gfn_to_pfn_memslot(slot, fault->gfn, false, NULL,
					  fault->write, &fault->map_writable,
					  &fault->hva);
	return false;

out_retry:
	*r = RET_PF_RETRY;
	return true;
}