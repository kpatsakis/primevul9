static int sas_find_sub_addr(struct domain_device *dev, u8 *sub_addr)
{
	struct expander_device *ex = &dev->ex_dev;
	int i;

	for (i = 0; i < ex->num_phys; i++) {
		struct ex_phy *phy = &ex->ex_phy[i];

		if (phy->phy_state == PHY_VACANT ||
		    phy->phy_state == PHY_NOT_PRESENT)
			continue;

		if ((phy->attached_dev_type == SAS_EDGE_EXPANDER_DEVICE ||
		     phy->attached_dev_type == SAS_FANOUT_EXPANDER_DEVICE) &&
		    phy->routing_attr == SUBTRACTIVE_ROUTING) {

			memcpy(sub_addr, phy->attached_sas_addr, SAS_ADDR_SIZE);

			return 1;
		}
	}
	return 0;
}