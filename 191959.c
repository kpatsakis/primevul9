static void rtl8723bu_set_ps_tdma(struct rtl8xxxu_priv *priv,
				  u8 arg1, u8 arg2, u8 arg3, u8 arg4, u8 arg5)
{
	struct h2c_cmd h2c;

	memset(&h2c, 0, sizeof(struct h2c_cmd));
	h2c.b_type_dma.cmd = H2C_8723B_B_TYPE_TDMA;
	h2c.b_type_dma.data1 = arg1;
	h2c.b_type_dma.data2 = arg2;
	h2c.b_type_dma.data3 = arg3;
	h2c.b_type_dma.data4 = arg4;
	h2c.b_type_dma.data5 = arg5;
	rtl8xxxu_gen2_h2c_cmd(priv, &h2c, sizeof(h2c.b_type_dma));
}