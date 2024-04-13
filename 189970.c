static void reset_guest_paging_metadata(struct kvm_vcpu *vcpu,
					struct kvm_mmu *mmu)
{
	if (!is_cr0_pg(mmu))
		return;

	reset_rsvds_bits_mask(vcpu, mmu);
	update_permission_bitmask(mmu, false);
	update_pkru_bitmask(mmu);
}