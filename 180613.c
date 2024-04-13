static int iommu_alloc_root_entry(struct intel_iommu *iommu)
{
	struct root_entry *root;
	unsigned long flags;

	root = (struct root_entry *)alloc_pgtable_page(iommu->node);
	if (!root) {
		pr_err("Allocating root entry for %s failed\n",
			iommu->name);
		return -ENOMEM;
	}

	__iommu_flush_cache(iommu, root, ROOT_SIZE);

	spin_lock_irqsave(&iommu->lock, flags);
	iommu->root_entry = root;
	spin_unlock_irqrestore(&iommu->lock, flags);

	return 0;
}