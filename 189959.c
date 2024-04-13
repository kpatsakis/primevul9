static void free_mmu_pages(struct kvm_mmu *mmu)
{
	if (!tdp_enabled && mmu->pae_root)
		set_memory_encrypted((unsigned long)mmu->pae_root, 1);
	free_page((unsigned long)mmu->pae_root);
	free_page((unsigned long)mmu->pml4_root);
	free_page((unsigned long)mmu->pml5_root);
}