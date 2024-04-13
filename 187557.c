static int sas_configure_parent(struct domain_device *parent,
				struct domain_device *child,
				u8 *sas_addr, int include)
{
	struct expander_device *ex_parent = &parent->ex_dev;
	int res = 0;
	int i;

	if (parent->parent) {
		res = sas_configure_parent(parent->parent, parent, sas_addr,
					   include);
		if (res)
			return res;
	}

	if (ex_parent->conf_route_table == 0) {
		pr_debug("ex %016llx has self-configuring routing table\n",
			 SAS_ADDR(parent->sas_addr));
		return 0;
	}

	for (i = 0; i < ex_parent->num_phys; i++) {
		struct ex_phy *phy = &ex_parent->ex_phy[i];

		if ((phy->routing_attr == TABLE_ROUTING) &&
		    (SAS_ADDR(phy->attached_sas_addr) ==
		     SAS_ADDR(child->sas_addr))) {
			res = sas_configure_phy(parent, i, sas_addr, include);
			if (res)
				return res;
		}
	}

	return res;
}