static void domain_reserve_special_ranges(struct dmar_domain *domain)
{
	copy_reserved_iova(&reserved_iova_list, &domain->iovad);
}