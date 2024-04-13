static int domain_mapping(struct dmar_domain *domain, unsigned long iov_pfn,
                         struct scatterlist *sg, unsigned long phys_pfn,
                         unsigned long nr_pages, int prot)
{
       int ret;
       struct intel_iommu *iommu;

       /* Do the real mapping first */
       ret = __domain_mapping(domain, iov_pfn, sg, phys_pfn, nr_pages, prot);
       if (ret)
               return ret;

       /* Notify about the new mapping */
       if (domain_type_is_vm(domain)) {
	       /* VM typed domains can have more than one IOMMUs */
	       int iommu_id;
	       for_each_domain_iommu(iommu_id, domain) {
		       iommu = g_iommus[iommu_id];
		       __mapping_notify_one(iommu, domain, iov_pfn, nr_pages);
	       }
       } else {
	       /* General domains only have one IOMMU */
	       iommu = domain_get_iommu(domain);
	       __mapping_notify_one(iommu, domain, iov_pfn, nr_pages);
       }

       return 0;
}