static bool spte_write_protect(u64 *sptep, bool pt_protect)
{
	u64 spte = *sptep;

	if (!is_writable_pte(spte) &&
	    !(pt_protect && is_mmu_writable_spte(spte)))
		return false;

	rmap_printk("spte %p %llx\n", sptep, *sptep);

	if (pt_protect)
		spte &= ~shadow_mmu_writable_mask;
	spte = spte & ~PT_WRITABLE_MASK;

	return mmu_spte_update(sptep, spte);
}