static void intel_iommu_put_resv_regions(struct device *dev,
					 struct list_head *head)
{
	struct iommu_resv_region *entry, *next;

	list_for_each_entry_safe(entry, next, head, list) {
		if (entry->type == IOMMU_RESV_RESERVED)
			kfree(entry);
	}
}