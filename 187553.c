static void sas_ex_disable_phy(struct domain_device *dev, int phy_id)
{
	struct expander_device *ex = &dev->ex_dev;
	struct ex_phy *phy = &ex->ex_phy[phy_id];

	sas_smp_phy_control(dev, phy_id, PHY_FUNC_DISABLE, NULL);
	phy->linkrate = SAS_PHY_DISABLED;
}