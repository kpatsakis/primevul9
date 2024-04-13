static bool spte_clear_dirty(u64 *sptep)
{
	u64 spte = *sptep;

	rmap_printk("spte %p %llx\n", sptep, *sptep);

	MMU_WARN_ON(!spte_ad_enabled(spte));
	spte &= ~shadow_dirty_mask;
	return mmu_spte_update(sptep, spte);
}