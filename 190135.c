static gfn_t get_mmio_spte_gfn(u64 spte)
{
	u64 gpa = spte & shadow_nonpresent_or_rsvd_lower_gfn_mask;

	gpa |= (spte >> SHADOW_NONPRESENT_OR_RSVD_MASK_LEN)
	       & shadow_nonpresent_or_rsvd_mask;

	return gpa >> PAGE_SHIFT;
}