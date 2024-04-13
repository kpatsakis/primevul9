ax88179_ethtool_get_eee(struct usbnet *dev, struct ethtool_eee *data)
{
	int val;

	/* Get Supported EEE */
	val = ax88179_phy_read_mmd_indirect(dev, MDIO_PCS_EEE_ABLE,
					    MDIO_MMD_PCS);
	if (val < 0)
		return val;
	data->supported = mmd_eee_cap_to_ethtool_sup_t(val);

	/* Get advertisement EEE */
	val = ax88179_phy_read_mmd_indirect(dev, MDIO_AN_EEE_ADV,
					    MDIO_MMD_AN);
	if (val < 0)
		return val;
	data->advertised = mmd_eee_adv_to_ethtool_adv_t(val);

	/* Get LP advertisement EEE */
	val = ax88179_phy_read_mmd_indirect(dev, MDIO_AN_EEE_LPABLE,
					    MDIO_MMD_AN);
	if (val < 0)
		return val;
	data->lp_advertised = mmd_eee_adv_to_ethtool_adv_t(val);

	return 0;
}