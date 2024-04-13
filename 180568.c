static void domain_exit(struct dmar_domain *domain)
{
	struct page *freelist = NULL;

	/* Domain 0 is reserved, so dont process it */
	if (!domain)
		return;

	/* Remove associated devices and clear attached or cached domains */
	rcu_read_lock();
	domain_remove_dev_info(domain);
	rcu_read_unlock();

	/* destroy iovas */
	put_iova_domain(&domain->iovad);

	freelist = domain_unmap(domain, 0, DOMAIN_MAX_PFN(domain->gaw));

	dma_free_pagelist(freelist);

	free_domain_mem(domain);
}