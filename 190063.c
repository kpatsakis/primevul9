void kvm_configure_mmu(bool enable_tdp, int tdp_forced_root_level,
		       int tdp_max_root_level, int tdp_huge_page_level)
{
	tdp_enabled = enable_tdp;
	tdp_root_level = tdp_forced_root_level;
	max_tdp_level = tdp_max_root_level;

	/*
	 * max_huge_page_level reflects KVM's MMU capabilities irrespective
	 * of kernel support, e.g. KVM may be capable of using 1GB pages when
	 * the kernel is not.  But, KVM never creates a page size greater than
	 * what is used by the kernel for any given HVA, i.e. the kernel's
	 * capabilities are ultimately consulted by kvm_mmu_hugepage_adjust().
	 */
	if (tdp_enabled)
		max_huge_page_level = tdp_huge_page_level;
	else if (boot_cpu_has(X86_FEATURE_GBPAGES))
		max_huge_page_level = PG_LEVEL_1G;
	else
		max_huge_page_level = PG_LEVEL_2M;
}