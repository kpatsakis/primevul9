struct domain_device *sas_ex_to_ata(struct domain_device *ex_dev, int phy_id)
{
	struct ex_phy *ex_phy = &ex_dev->ex_dev.ex_phy[phy_id];
	struct domain_device *dev;
	struct sas_rphy *rphy;

	if (!ex_phy->port)
		return NULL;

	rphy = ex_phy->port->rphy;
	if (!rphy)
		return NULL;

	dev = sas_find_dev_by_rphy(rphy);

	if (dev && dev_is_sata(dev))
		return dev;

	return NULL;
}