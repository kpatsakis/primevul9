int kvm_mmu_page_fault(struct kvm_vcpu *vcpu, gpa_t cr2_or_gpa, u64 error_code,
		       void *insn, int insn_len)
{
	int r, emulation_type = EMULTYPE_PF;
	bool direct = vcpu->arch.mmu->direct_map;

	if (WARN_ON(!VALID_PAGE(vcpu->arch.mmu->root.hpa)))
		return RET_PF_RETRY;

	r = RET_PF_INVALID;
	if (unlikely(error_code & PFERR_RSVD_MASK)) {
		r = handle_mmio_page_fault(vcpu, cr2_or_gpa, direct);
		if (r == RET_PF_EMULATE)
			goto emulate;
	}

	if (r == RET_PF_INVALID) {
		r = kvm_mmu_do_page_fault(vcpu, cr2_or_gpa,
					  lower_32_bits(error_code), false);
		if (KVM_BUG_ON(r == RET_PF_INVALID, vcpu->kvm))
			return -EIO;
	}

	if (r < 0)
		return r;
	if (r != RET_PF_EMULATE)
		return 1;

	/*
	 * Before emulating the instruction, check if the error code
	 * was due to a RO violation while translating the guest page.
	 * This can occur when using nested virtualization with nested
	 * paging in both guests. If true, we simply unprotect the page
	 * and resume the guest.
	 */
	if (vcpu->arch.mmu->direct_map &&
	    (error_code & PFERR_NESTED_GUEST_PAGE) == PFERR_NESTED_GUEST_PAGE) {
		kvm_mmu_unprotect_page(vcpu->kvm, gpa_to_gfn(cr2_or_gpa));
		return 1;
	}

	/*
	 * vcpu->arch.mmu.page_fault returned RET_PF_EMULATE, but we can still
	 * optimistically try to just unprotect the page and let the processor
	 * re-execute the instruction that caused the page fault.  Do not allow
	 * retrying MMIO emulation, as it's not only pointless but could also
	 * cause us to enter an infinite loop because the processor will keep
	 * faulting on the non-existent MMIO address.  Retrying an instruction
	 * from a nested guest is also pointless and dangerous as we are only
	 * explicitly shadowing L1's page tables, i.e. unprotecting something
	 * for L1 isn't going to magically fix whatever issue cause L2 to fail.
	 */
	if (!mmio_info_in_cache(vcpu, cr2_or_gpa, direct) && !is_guest_mode(vcpu))
		emulation_type |= EMULTYPE_ALLOW_RETRY_PF;
emulate:
	return x86_emulate_instruction(vcpu, cr2_or_gpa, emulation_type, insn,
				       insn_len);
}