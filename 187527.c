static void sas_ex_disable_port(struct domain_device *dev, u8 *sas_addr)
{
	struct expander_device *ex = &dev->ex_dev;
	int i;

	for (i = 0; i < ex->num_phys; i++) {
		struct ex_phy *phy = &ex->ex_phy[i];

		if (phy->phy_state == PHY_VACANT ||
		    phy->phy_state == PHY_NOT_PRESENT)
			continue;

		if (SAS_ADDR(phy->attached_sas_addr) == SAS_ADDR(sas_addr))
			sas_ex_disable_phy(dev, i);
	}
}