void rtl8xxxu_gen1_report_connect(struct rtl8xxxu_priv *priv,
				  u8 macid, bool connect)
{
	struct h2c_cmd h2c;

	memset(&h2c, 0, sizeof(struct h2c_cmd));

	h2c.joinbss.cmd = H2C_JOIN_BSS_REPORT;

	if (connect)
		h2c.joinbss.data = H2C_JOIN_BSS_CONNECT;
	else
		h2c.joinbss.data = H2C_JOIN_BSS_DISCONNECT;

	rtl8xxxu_gen1_h2c_cmd(priv, &h2c, sizeof(h2c.joinbss));
}