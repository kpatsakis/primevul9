int kvm_mmu_vendor_module_init(void)
{
	int ret = -ENOMEM;

	/*
	 * MMU roles use union aliasing which is, generally speaking, an
	 * undefined behavior. However, we supposedly know how compilers behave
	 * and the current status quo is unlikely to change. Guardians below are
	 * supposed to let us know if the assumption becomes false.
	 */
	BUILD_BUG_ON(sizeof(union kvm_mmu_page_role) != sizeof(u32));
	BUILD_BUG_ON(sizeof(union kvm_mmu_extended_role) != sizeof(u32));
	BUILD_BUG_ON(sizeof(union kvm_mmu_role) != sizeof(u64));

	kvm_mmu_reset_all_pte_masks();

	pte_list_desc_cache = kmem_cache_create("pte_list_desc",
					    sizeof(struct pte_list_desc),
					    0, SLAB_ACCOUNT, NULL);
	if (!pte_list_desc_cache)
		goto out;

	mmu_page_header_cache = kmem_cache_create("kvm_mmu_page_header",
						  sizeof(struct kvm_mmu_page),
						  0, SLAB_ACCOUNT, NULL);
	if (!mmu_page_header_cache)
		goto out;

	if (percpu_counter_init(&kvm_total_used_mmu_pages, 0, GFP_KERNEL))
		goto out;

	ret = register_shrinker(&mmu_shrinker);
	if (ret)
		goto out;

	return 0;

out:
	mmu_destroy_caches();
	return ret;
}