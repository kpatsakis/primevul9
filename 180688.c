static inline int domain_sg_mapping(struct dmar_domain *domain, unsigned long iov_pfn,
				    struct scatterlist *sg, unsigned long nr_pages,
				    int prot)
{
	return domain_mapping(domain, iov_pfn, sg, 0, nr_pages, prot);
}