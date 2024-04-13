void rtl8xxxu_gen2_prepare_calibrate(struct rtl8xxxu_priv *priv, u8 start)
{
	struct h2c_cmd h2c;

	memset(&h2c, 0, sizeof(struct h2c_cmd));
	h2c.bt_wlan_calibration.cmd = H2C_8723B_BT_WLAN_CALIBRATION;
	h2c.bt_wlan_calibration.data = start;

	rtl8xxxu_gen2_h2c_cmd(priv, &h2c, sizeof(h2c.bt_wlan_calibration));
}