static inline int ax88179_phy_mmd_indirect(struct usbnet *dev, u16 prtad,
					   u16 devad)
{
	u16 tmp16;
	int ret;

	tmp16 = devad;
	ret = ax88179_write_cmd(dev, AX_ACCESS_PHY, AX88179_PHY_ID,
				MII_MMD_CTRL, 2, &tmp16);

	tmp16 = prtad;
	ret = ax88179_write_cmd(dev, AX_ACCESS_PHY, AX88179_PHY_ID,
				MII_MMD_DATA, 2, &tmp16);

	tmp16 = devad | MII_MMD_CTRL_NOINCR;
	ret = ax88179_write_cmd(dev, AX_ACCESS_PHY, AX88179_PHY_ID,
				MII_MMD_CTRL, 2, &tmp16);

	return ret;
}