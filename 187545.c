static int sas_check_eeds(struct domain_device *child,
				 struct ex_phy *parent_phy,
				 struct ex_phy *child_phy)
{
	int res = 0;
	struct domain_device *parent = child->parent;

	if (SAS_ADDR(parent->port->disc.fanout_sas_addr) != 0) {
		res = -ENODEV;
		pr_warn("edge ex %016llx phy S:%02d <--> edge ex %016llx phy S:%02d, while there is a fanout ex %016llx\n",
			SAS_ADDR(parent->sas_addr),
			parent_phy->phy_id,
			SAS_ADDR(child->sas_addr),
			child_phy->phy_id,
			SAS_ADDR(parent->port->disc.fanout_sas_addr));
	} else if (SAS_ADDR(parent->port->disc.eeds_a) == 0) {
		memcpy(parent->port->disc.eeds_a, parent->sas_addr,
		       SAS_ADDR_SIZE);
		memcpy(parent->port->disc.eeds_b, child->sas_addr,
		       SAS_ADDR_SIZE);
	} else if (((SAS_ADDR(parent->port->disc.eeds_a) ==
		    SAS_ADDR(parent->sas_addr)) ||
		   (SAS_ADDR(parent->port->disc.eeds_a) ==
		    SAS_ADDR(child->sas_addr)))
		   &&
		   ((SAS_ADDR(parent->port->disc.eeds_b) ==
		     SAS_ADDR(parent->sas_addr)) ||
		    (SAS_ADDR(parent->port->disc.eeds_b) ==
		     SAS_ADDR(child->sas_addr))))
		;
	else {
		res = -ENODEV;
		pr_warn("edge ex %016llx phy%02d <--> edge ex %016llx phy%02d link forms a third EEDS!\n",
			SAS_ADDR(parent->sas_addr),
			parent_phy->phy_id,
			SAS_ADDR(child->sas_addr),
			child_phy->phy_id);
	}

	return res;
}