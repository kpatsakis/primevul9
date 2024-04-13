static inline int domain_type_is_si(struct dmar_domain *domain)
{
	return domain->flags & DOMAIN_FLAG_STATIC_IDENTITY;
}