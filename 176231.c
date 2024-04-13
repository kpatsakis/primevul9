ax88179_ethtool_set_eee(struct usbnet *dev, struct ethtool_eee *data)
{
	u16 tmp16 = ethtool_adv_to_mmd_eee_adv_t(data->advertised);

	return ax88179_phy_write_mmd_indirect(dev, MDIO_AN_EEE_ADV,
					      MDIO_MMD_AN, tmp16);
}