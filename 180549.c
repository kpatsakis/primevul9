static int domain_attach_iommu(struct dmar_domain *domain,
			       struct intel_iommu *iommu)
{
	unsigned long ndomains;
	int num;

	assert_spin_locked(&device_domain_lock);
	assert_spin_locked(&iommu->lock);

	domain->iommu_refcnt[iommu->seq_id] += 1;
	domain->iommu_count += 1;
	if (domain->iommu_refcnt[iommu->seq_id] == 1) {
		ndomains = cap_ndoms(iommu->cap);
		num      = find_first_zero_bit(iommu->domain_ids, ndomains);

		if (num >= ndomains) {
			pr_err("%s: No free domain ids\n", iommu->name);
			domain->iommu_refcnt[iommu->seq_id] -= 1;
			domain->iommu_count -= 1;
			return -ENOSPC;
		}

		set_bit(num, iommu->domain_ids);
		set_iommu_domain(iommu, num, domain);

		domain->iommu_did[iommu->seq_id] = num;
		domain->nid			 = iommu->node;

		domain_update_iommu_cap(domain);
	}

	return 0;
}