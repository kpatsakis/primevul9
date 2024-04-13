static int sas_check_ex_subtractive_boundary(struct domain_device *dev)
{
	struct expander_device *ex = &dev->ex_dev;
	int i;
	u8  *sub_sas_addr = NULL;

	if (dev->dev_type != SAS_EDGE_EXPANDER_DEVICE)
		return 0;

	for (i = 0; i < ex->num_phys; i++) {
		struct ex_phy *phy = &ex->ex_phy[i];

		if (phy->phy_state == PHY_VACANT ||
		    phy->phy_state == PHY_NOT_PRESENT)
			continue;

		if ((phy->attached_dev_type == SAS_FANOUT_EXPANDER_DEVICE ||
		     phy->attached_dev_type == SAS_EDGE_EXPANDER_DEVICE) &&
		    phy->routing_attr == SUBTRACTIVE_ROUTING) {

			if (!sub_sas_addr)
				sub_sas_addr = &phy->attached_sas_addr[0];
			else if (SAS_ADDR(sub_sas_addr) !=
				 SAS_ADDR(phy->attached_sas_addr)) {

				pr_notice("ex %016llx phy%02d diverges(%016llx) on subtractive boundary(%016llx). Disabled\n",
					  SAS_ADDR(dev->sas_addr), i,
					  SAS_ADDR(phy->attached_sas_addr),
					  SAS_ADDR(sub_sas_addr));
				sas_ex_disable_phy(dev, i);
			}
		}
	}
	return 0;
}