reset_tdp_shadow_zero_bits_mask(struct kvm_mmu *context)
{
	struct rsvd_bits_validate *shadow_zero_check;
	int i;

	shadow_zero_check = &context->shadow_zero_check;

	if (boot_cpu_is_amd())
		__reset_rsvds_bits_mask(shadow_zero_check, reserved_hpa_bits(),
					context->shadow_root_level, false,
					boot_cpu_has(X86_FEATURE_GBPAGES),
					false, true);
	else
		__reset_rsvds_bits_mask_ept(shadow_zero_check,
					    reserved_hpa_bits(), false,
					    max_huge_page_level);

	if (!shadow_me_mask)
		return;

	for (i = context->shadow_root_level; --i >= 0;) {
		shadow_zero_check->rsvd_bits_mask[0][i] &= ~shadow_me_mask;
		shadow_zero_check->rsvd_bits_mask[1][i] &= ~shadow_me_mask;
	}
}