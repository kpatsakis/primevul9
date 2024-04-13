static int sas_configure_phy(struct domain_device *dev, int phy_id,
				    u8 *sas_addr, int include)
{
	int index;
	int present;
	int res;

	res = sas_configure_present(dev, phy_id, sas_addr, &index, &present);
	if (res)
		return res;
	if (include ^ present)
		return sas_configure_set(dev, phy_id, sas_addr, index,include);

	return res;
}