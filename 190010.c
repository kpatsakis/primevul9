static int host_pfn_mapping_level(struct kvm *kvm, gfn_t gfn, kvm_pfn_t pfn,
				  const struct kvm_memory_slot *slot)
{
	unsigned long hva;
	unsigned long flags;
	int level = PG_LEVEL_4K;
	pgd_t pgd;
	p4d_t p4d;
	pud_t pud;
	pmd_t pmd;

	if (!PageCompound(pfn_to_page(pfn)) && !kvm_is_zone_device_pfn(pfn))
		return PG_LEVEL_4K;

	/*
	 * Note, using the already-retrieved memslot and __gfn_to_hva_memslot()
	 * is not solely for performance, it's also necessary to avoid the
	 * "writable" check in __gfn_to_hva_many(), which will always fail on
	 * read-only memslots due to gfn_to_hva() assuming writes.  Earlier
	 * page fault steps have already verified the guest isn't writing a
	 * read-only memslot.
	 */
	hva = __gfn_to_hva_memslot(slot, gfn);

	/*
	 * Lookup the mapping level in the current mm.  The information
	 * may become stale soon, but it is safe to use as long as
	 * 1) mmu_notifier_retry was checked after taking mmu_lock, and
	 * 2) mmu_lock is taken now.
	 *
	 * We still need to disable IRQs to prevent concurrent tear down
	 * of page tables.
	 */
	local_irq_save(flags);

	pgd = READ_ONCE(*pgd_offset(kvm->mm, hva));
	if (pgd_none(pgd))
		goto out;

	p4d = READ_ONCE(*p4d_offset(&pgd, hva));
	if (p4d_none(p4d) || !p4d_present(p4d))
		goto out;

	pud = READ_ONCE(*pud_offset(&p4d, hva));
	if (pud_none(pud) || !pud_present(pud))
		goto out;

	if (pud_large(pud)) {
		level = PG_LEVEL_1G;
		goto out;
	}

	pmd = READ_ONCE(*pmd_offset(&pud, hva));
	if (pmd_none(pmd) || !pmd_present(pmd))
		goto out;

	if (pmd_large(pmd))
		level = PG_LEVEL_2M;

out:
	local_irq_restore(flags);
	return level;
}