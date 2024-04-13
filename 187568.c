static int sas_rediscover(struct domain_device *dev, const int phy_id)
{
	struct expander_device *ex = &dev->ex_dev;
	struct ex_phy *changed_phy = &ex->ex_phy[phy_id];
	int res = 0;
	int i;
	bool last = true;	/* is this the last phy of the port */

	pr_debug("ex %016llx phy%02d originated BROADCAST(CHANGE)\n",
		 SAS_ADDR(dev->sas_addr), phy_id);

	if (SAS_ADDR(changed_phy->attached_sas_addr) != 0) {
		for (i = 0; i < ex->num_phys; i++) {
			struct ex_phy *phy = &ex->ex_phy[i];

			if (i == phy_id)
				continue;
			if (SAS_ADDR(phy->attached_sas_addr) ==
			    SAS_ADDR(changed_phy->attached_sas_addr)) {
				last = false;
				break;
			}
		}
		res = sas_rediscover_dev(dev, phy_id, last, i);
	} else
		res = sas_discover_new(dev, phy_id);
	return res;
}