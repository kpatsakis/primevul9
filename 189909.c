static void reset_rsvds_bits_mask(struct kvm_vcpu *vcpu,
				  struct kvm_mmu *context)
{
	__reset_rsvds_bits_mask(&context->guest_rsvd_check,
				vcpu->arch.reserved_gpa_bits,
				context->root_level, is_efer_nx(context),
				guest_can_use_gbpages(vcpu),
				is_cr4_pse(context),
				guest_cpuid_is_amd_or_hygon(vcpu));
}