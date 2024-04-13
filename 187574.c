static int sas_ex_discover_dev(struct domain_device *dev, int phy_id)
{
	struct expander_device *ex = &dev->ex_dev;
	struct ex_phy *ex_phy = &ex->ex_phy[phy_id];
	struct domain_device *child = NULL;
	int res = 0;

	/* Phy state */
	if (ex_phy->linkrate == SAS_SATA_SPINUP_HOLD) {
		if (!sas_smp_phy_control(dev, phy_id, PHY_FUNC_LINK_RESET, NULL))
			res = sas_ex_phy_discover(dev, phy_id);
		if (res)
			return res;
	}

	/* Parent and domain coherency */
	if (!dev->parent && (SAS_ADDR(ex_phy->attached_sas_addr) ==
			     SAS_ADDR(dev->port->sas_addr))) {
		sas_add_parent_port(dev, phy_id);
		return 0;
	}
	if (dev->parent && (SAS_ADDR(ex_phy->attached_sas_addr) ==
			    SAS_ADDR(dev->parent->sas_addr))) {
		sas_add_parent_port(dev, phy_id);
		if (ex_phy->routing_attr == TABLE_ROUTING)
			sas_configure_phy(dev, phy_id, dev->port->sas_addr, 1);
		return 0;
	}

	if (sas_dev_present_in_domain(dev->port, ex_phy->attached_sas_addr))
		sas_ex_disable_port(dev, ex_phy->attached_sas_addr);

	if (ex_phy->attached_dev_type == SAS_PHY_UNUSED) {
		if (ex_phy->routing_attr == DIRECT_ROUTING) {
			memset(ex_phy->attached_sas_addr, 0, SAS_ADDR_SIZE);
			sas_configure_routing(dev, ex_phy->attached_sas_addr);
		}
		return 0;
	} else if (ex_phy->linkrate == SAS_LINK_RATE_UNKNOWN)
		return 0;

	if (ex_phy->attached_dev_type != SAS_END_DEVICE &&
	    ex_phy->attached_dev_type != SAS_FANOUT_EXPANDER_DEVICE &&
	    ex_phy->attached_dev_type != SAS_EDGE_EXPANDER_DEVICE &&
	    ex_phy->attached_dev_type != SAS_SATA_PENDING) {
		pr_warn("unknown device type(0x%x) attached to ex %016llx phy%02d\n",
			ex_phy->attached_dev_type,
			SAS_ADDR(dev->sas_addr),
			phy_id);
		return 0;
	}

	res = sas_configure_routing(dev, ex_phy->attached_sas_addr);
	if (res) {
		pr_notice("configure routing for dev %016llx reported 0x%x. Forgotten\n",
			  SAS_ADDR(ex_phy->attached_sas_addr), res);
		sas_disable_routing(dev, ex_phy->attached_sas_addr);
		return res;
	}

	if (sas_ex_join_wide_port(dev, phy_id)) {
		pr_debug("Attaching ex phy%02d to wide port %016llx\n",
			 phy_id, SAS_ADDR(ex_phy->attached_sas_addr));
		return res;
	}

	switch (ex_phy->attached_dev_type) {
	case SAS_END_DEVICE:
	case SAS_SATA_PENDING:
		child = sas_ex_discover_end_dev(dev, phy_id);
		break;
	case SAS_FANOUT_EXPANDER_DEVICE:
		if (SAS_ADDR(dev->port->disc.fanout_sas_addr)) {
			pr_debug("second fanout expander %016llx phy%02d attached to ex %016llx phy%02d\n",
				 SAS_ADDR(ex_phy->attached_sas_addr),
				 ex_phy->attached_phy_id,
				 SAS_ADDR(dev->sas_addr),
				 phy_id);
			sas_ex_disable_phy(dev, phy_id);
			break;
		} else
			memcpy(dev->port->disc.fanout_sas_addr,
			       ex_phy->attached_sas_addr, SAS_ADDR_SIZE);
		/* fallthrough */
	case SAS_EDGE_EXPANDER_DEVICE:
		child = sas_ex_discover_expander(dev, phy_id);
		break;
	default:
		break;
	}

	if (child) {
		int i;

		for (i = 0; i < ex->num_phys; i++) {
			if (ex->ex_phy[i].phy_state == PHY_VACANT ||
			    ex->ex_phy[i].phy_state == PHY_NOT_PRESENT)
				continue;
			/*
			 * Due to races, the phy might not get added to the
			 * wide port, so we add the phy to the wide port here.
			 */
			if (SAS_ADDR(ex->ex_phy[i].attached_sas_addr) ==
			    SAS_ADDR(child->sas_addr)) {
				ex->ex_phy[i].phy_state= PHY_DEVICE_DISCOVERED;
				if (sas_ex_join_wide_port(dev, i))
					pr_debug("Attaching ex phy%02d to wide port %016llx\n",
						 i, SAS_ADDR(ex->ex_phy[i].attached_sas_addr));
			}
		}
	}

	return res;
}