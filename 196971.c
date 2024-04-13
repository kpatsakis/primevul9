static bool FNAME(prefetch_invalid_gpte)(struct kvm_vcpu *vcpu,
				  struct kvm_mmu_page *sp, u64 *spte,
				  u64 gpte)
{
	if (is_rsvd_bits_set(vcpu->arch.mmu, gpte, PT_PAGE_TABLE_LEVEL))
		goto no_present;

	if (!FNAME(is_present_gpte)(gpte))
		goto no_present;

	/* if accessed bit is not supported prefetch non accessed gpte */
	if (PT_HAVE_ACCESSED_DIRTY(vcpu->arch.mmu) &&
	    !(gpte & PT_GUEST_ACCESSED_MASK))
		goto no_present;

	return false;

no_present:
	drop_spte(vcpu->kvm, spte);
	return true;
}