static inline int domain_type_is_vm(struct dmar_domain *domain)
{
	return domain->flags & DOMAIN_FLAG_VIRTUAL_MACHINE;
}