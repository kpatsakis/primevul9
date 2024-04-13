static int rtl8xxxu_set_bssid(struct rtl8xxxu_priv *priv, const u8 *bssid)
{
	int i;
	u16 reg;

	dev_dbg(&priv->udev->dev, "%s: (%pM)\n", __func__, bssid);

	reg = REG_BSSID;

	for (i = 0; i < ETH_ALEN; i++)
		rtl8xxxu_write8(priv, reg + i, bssid[i]);

	return 0;
}