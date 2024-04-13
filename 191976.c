static int rtl8xxxu_set_mac(struct rtl8xxxu_priv *priv)
{
	int i;
	u16 reg;

	reg = REG_MACID;

	for (i = 0; i < ETH_ALEN; i++)
		rtl8xxxu_write8(priv, reg + i, priv->mac_addr[i]);

	return 0;
}