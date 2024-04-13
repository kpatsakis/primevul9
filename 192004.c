static void rtl8xxxu_cam_write(struct rtl8xxxu_priv *priv,
			       struct ieee80211_key_conf *key, const u8 *mac)
{
	u32 cmd, val32, addr, ctrl;
	int j, i, tmp_debug;

	tmp_debug = rtl8xxxu_debug;
	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_KEY)
		rtl8xxxu_debug |= RTL8XXXU_DEBUG_REG_WRITE;

	/*
	 * This is a bit of a hack - the lower bits of the cipher
	 * suite selector happens to match the cipher index in the CAM
	 */
	addr = key->keyidx << CAM_CMD_KEY_SHIFT;
	ctrl = (key->cipher & 0x0f) << 2 | key->keyidx | CAM_WRITE_VALID;

	for (j = 5; j >= 0; j--) {
		switch (j) {
		case 0:
			val32 = ctrl | (mac[0] << 16) | (mac[1] << 24);
			break;
		case 1:
			val32 = mac[2] | (mac[3] << 8) |
				(mac[4] << 16) | (mac[5] << 24);
			break;
		default:
			i = (j - 2) << 2;
			val32 = key->key[i] | (key->key[i + 1] << 8) |
				key->key[i + 2] << 16 | key->key[i + 3] << 24;
			break;
		}

		rtl8xxxu_write32(priv, REG_CAM_WRITE, val32);
		cmd = CAM_CMD_POLLING | CAM_CMD_WRITE | (addr + j);
		rtl8xxxu_write32(priv, REG_CAM_CMD, cmd);
		udelay(100);
	}

	rtl8xxxu_debug = tmp_debug;
}