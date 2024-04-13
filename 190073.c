static int handle_mmio_page_fault(struct kvm_vcpu *vcpu, u64 addr, bool direct)
{
	u64 spte;
	bool reserved;

	if (mmio_info_in_cache(vcpu, addr, direct))
		return RET_PF_EMULATE;

	reserved = get_mmio_spte(vcpu, addr, &spte);
	if (WARN_ON(reserved))
		return -EINVAL;

	if (is_mmio_spte(spte)) {
		gfn_t gfn = get_mmio_spte_gfn(spte);
		unsigned int access = get_mmio_spte_access(spte);

		if (!check_mmio_spte(vcpu, spte))
			return RET_PF_INVALID;

		if (direct)
			addr = 0;

		trace_handle_mmio_page_fault(addr, gfn, access);
		vcpu_cache_mmio_info(vcpu, addr, gfn, access);
		return RET_PF_EMULATE;
	}

	/*
	 * If the page table is zapped by other cpus, let CPU fault again on
	 * the address.
	 */
	return RET_PF_RETRY;
}