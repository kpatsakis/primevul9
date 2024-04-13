reset_ept_shadow_zero_bits_mask(struct kvm_mmu *context, bool execonly)
{
	__reset_rsvds_bits_mask_ept(&context->shadow_zero_check,
				    reserved_hpa_bits(), execonly,
				    max_huge_page_level);
}