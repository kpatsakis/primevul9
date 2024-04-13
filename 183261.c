static int domain_detach_iommu(struct dmar_domain *domain,
			       struct intel_iommu *iommu)
{
	int num, count;

	assert_spin_locked(&device_domain_lock);
	assert_spin_locked(&iommu->lock);

	domain->iommu_refcnt[iommu->seq_id] -= 1;
	count = --domain->iommu_count;
	if (domain->iommu_refcnt[iommu->seq_id] == 0) {
		num = domain->iommu_did[iommu->seq_id];
		clear_bit(num, iommu->domain_ids);
		set_iommu_domain(iommu, num, NULL);

		domain_update_iommu_cap(domain);
		domain->iommu_did[iommu->seq_id] = 0;
	}

	return count;
}