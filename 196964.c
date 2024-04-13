static gpa_t FNAME(get_level1_sp_gpa)(struct kvm_mmu_page *sp)
{
	int offset = 0;

	WARN_ON(sp->role.level != PT_PAGE_TABLE_LEVEL);

	if (PTTYPE == 32)
		offset = sp->role.quadrant << PT64_LEVEL_BITS;

	return gfn_to_gpa(sp->gfn) + offset * sizeof(pt_element_t);
}