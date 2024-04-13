static void reset_rsvds_bits_mask_ept(struct kvm_vcpu *vcpu,
		struct kvm_mmu *context, bool execonly, int huge_page_level)
{
	__reset_rsvds_bits_mask_ept(&context->guest_rsvd_check,
				    vcpu->arch.reserved_gpa_bits, execonly,
				    huge_page_level);
}