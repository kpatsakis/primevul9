static void reset_shadow_zero_bits_mask(struct kvm_vcpu *vcpu,
					struct kvm_mmu *context)
{
	/*
	 * KVM uses NX when TDP is disabled to handle a variety of scenarios,
	 * notably for huge SPTEs if iTLB multi-hit mitigation is enabled and
	 * to generate correct permissions for CR0.WP=0/CR4.SMEP=1/EFER.NX=0.
	 * The iTLB multi-hit workaround can be toggled at any time, so assume
	 * NX can be used by any non-nested shadow MMU to avoid having to reset
	 * MMU contexts.  Note, KVM forces EFER.NX=1 when TDP is disabled.
	 */
	bool uses_nx = is_efer_nx(context) || !tdp_enabled;

	/* @amd adds a check on bit of SPTEs, which KVM shouldn't use anyways. */
	bool is_amd = true;
	/* KVM doesn't use 2-level page tables for the shadow MMU. */
	bool is_pse = false;
	struct rsvd_bits_validate *shadow_zero_check;
	int i;

	WARN_ON_ONCE(context->shadow_root_level < PT32E_ROOT_LEVEL);

	shadow_zero_check = &context->shadow_zero_check;
	__reset_rsvds_bits_mask(shadow_zero_check, reserved_hpa_bits(),
				context->shadow_root_level, uses_nx,
				guest_can_use_gbpages(vcpu), is_pse, is_amd);

	if (!shadow_me_mask)
		return;

	for (i = context->shadow_root_level; --i >= 0;) {
		shadow_zero_check->rsvd_bits_mask[0][i] &= ~shadow_me_mask;
		shadow_zero_check->rsvd_bits_mask[1][i] &= ~shadow_me_mask;
	}

}