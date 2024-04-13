static gfn_t kvm_mmu_page_get_gfn(struct kvm_mmu_page *sp, int index)
{
	if (!sp->role.direct)
		return sp->gfns[index];

	return sp->gfn + (index << ((sp->role.level - 1) * PT64_LEVEL_BITS));
}