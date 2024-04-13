struct dmar_domain *get_valid_domain_for_dev(struct device *dev)
{
	struct dmar_domain *domain, *tmp;
	struct dmar_rmrr_unit *rmrr;
	struct device *i_dev;
	int i, ret;

	domain = find_domain(dev);
	if (domain)
		goto out;

	domain = find_or_alloc_domain(dev, DEFAULT_DOMAIN_ADDRESS_WIDTH);
	if (!domain)
		goto out;

	/* We have a new domain - setup possible RMRRs for the device */
	rcu_read_lock();
	for_each_rmrr_units(rmrr) {
		for_each_active_dev_scope(rmrr->devices, rmrr->devices_cnt,
					  i, i_dev) {
			if (i_dev != dev)
				continue;

			ret = domain_prepare_identity_map(dev, domain,
							  rmrr->base_address,
							  rmrr->end_address);
			if (ret)
				dev_err(dev, "Mapping reserved region failed\n");
		}
	}
	rcu_read_unlock();

	tmp = set_domain_for_dev(dev, domain);
	if (!tmp || domain != tmp) {
		domain_exit(domain);
		domain = tmp;
	}

out:

	if (!domain)
		pr_err("Allocating domain for %s failed\n", dev_name(dev));


	return domain;
}