static inline int domain_pfn_mapping(struct dmar_domain *domain, unsigned long iov_pfn,
				     unsigned long phys_pfn, unsigned long nr_pages,
				     int prot)
{
	return domain_mapping(domain, iov_pfn, NULL, phys_pfn, nr_pages, prot);
}