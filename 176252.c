ax88179_phy_write_mmd_indirect(struct usbnet *dev, u16 prtad, u16 devad,
			       u16 data)
{
	int ret;

	ax88179_phy_mmd_indirect(dev, prtad, devad);

	ret = ax88179_write_cmd(dev, AX_ACCESS_PHY, AX88179_PHY_ID,
				MII_MMD_DATA, 2, &data);

	if (ret < 0)
		return ret;

	return 0;
}