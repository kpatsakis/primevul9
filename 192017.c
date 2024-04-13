rtl8xxxu_gen1_set_tx_power(struct rtl8xxxu_priv *priv, int channel, bool ht40)
{
	struct rtl8xxxu_power_base *power_base = priv->power_base;
	u8 cck[RTL8723A_MAX_RF_PATHS], ofdm[RTL8723A_MAX_RF_PATHS];
	u8 ofdmbase[RTL8723A_MAX_RF_PATHS], mcsbase[RTL8723A_MAX_RF_PATHS];
	u32 val32, ofdm_a, ofdm_b, mcs_a, mcs_b;
	u8 val8;
	int group, i;

	group = rtl8xxxu_gen1_channel_to_group(channel);

	cck[0] = priv->cck_tx_power_index_A[group] - 1;
	cck[1] = priv->cck_tx_power_index_B[group] - 1;

	if (priv->hi_pa) {
		if (cck[0] > 0x20)
			cck[0] = 0x20;
		if (cck[1] > 0x20)
			cck[1] = 0x20;
	}

	ofdm[0] = priv->ht40_1s_tx_power_index_A[group];
	ofdm[1] = priv->ht40_1s_tx_power_index_B[group];
	if (ofdm[0])
		ofdm[0] -= 1;
	if (ofdm[1])
		ofdm[1] -= 1;

	ofdmbase[0] = ofdm[0] +	priv->ofdm_tx_power_index_diff[group].a;
	ofdmbase[1] = ofdm[1] +	priv->ofdm_tx_power_index_diff[group].b;

	mcsbase[0] = ofdm[0];
	mcsbase[1] = ofdm[1];
	if (!ht40) {
		mcsbase[0] += priv->ht20_tx_power_index_diff[group].a;
		mcsbase[1] += priv->ht20_tx_power_index_diff[group].b;
	}

	if (priv->tx_paths > 1) {
		if (ofdm[0] > priv->ht40_2s_tx_power_index_diff[group].a)
			ofdm[0] -=  priv->ht40_2s_tx_power_index_diff[group].a;
		if (ofdm[1] > priv->ht40_2s_tx_power_index_diff[group].b)
			ofdm[1] -=  priv->ht40_2s_tx_power_index_diff[group].b;
	}

	if (rtl8xxxu_debug & RTL8XXXU_DEBUG_CHANNEL)
		dev_info(&priv->udev->dev,
			 "%s: Setting TX power CCK A: %02x, "
			 "CCK B: %02x, OFDM A: %02x, OFDM B: %02x\n",
			 __func__, cck[0], cck[1], ofdm[0], ofdm[1]);

	for (i = 0; i < RTL8723A_MAX_RF_PATHS; i++) {
		if (cck[i] > RF6052_MAX_TX_PWR)
			cck[i] = RF6052_MAX_TX_PWR;
		if (ofdm[i] > RF6052_MAX_TX_PWR)
			ofdm[i] = RF6052_MAX_TX_PWR;
	}

	val32 = rtl8xxxu_read32(priv, REG_TX_AGC_A_CCK1_MCS32);
	val32 &= 0xffff00ff;
	val32 |= (cck[0] << 8);
	rtl8xxxu_write32(priv, REG_TX_AGC_A_CCK1_MCS32, val32);

	val32 = rtl8xxxu_read32(priv, REG_TX_AGC_B_CCK11_A_CCK2_11);
	val32 &= 0xff;
	val32 |= ((cck[0] << 8) | (cck[0] << 16) | (cck[0] << 24));
	rtl8xxxu_write32(priv, REG_TX_AGC_B_CCK11_A_CCK2_11, val32);

	val32 = rtl8xxxu_read32(priv, REG_TX_AGC_B_CCK11_A_CCK2_11);
	val32 &= 0xffffff00;
	val32 |= cck[1];
	rtl8xxxu_write32(priv, REG_TX_AGC_B_CCK11_A_CCK2_11, val32);

	val32 = rtl8xxxu_read32(priv, REG_TX_AGC_B_CCK1_55_MCS32);
	val32 &= 0xff;
	val32 |= ((cck[1] << 8) | (cck[1] << 16) | (cck[1] << 24));
	rtl8xxxu_write32(priv, REG_TX_AGC_B_CCK1_55_MCS32, val32);

	ofdm_a = ofdmbase[0] | ofdmbase[0] << 8 |
		ofdmbase[0] << 16 | ofdmbase[0] << 24;
	ofdm_b = ofdmbase[1] | ofdmbase[1] << 8 |
		ofdmbase[1] << 16 | ofdmbase[1] << 24;

	rtl8xxxu_write32(priv, REG_TX_AGC_A_RATE18_06,
			 ofdm_a + power_base->reg_0e00);
	rtl8xxxu_write32(priv, REG_TX_AGC_B_RATE18_06,
			 ofdm_b + power_base->reg_0830);

	rtl8xxxu_write32(priv, REG_TX_AGC_A_RATE54_24,
			 ofdm_a + power_base->reg_0e04);
	rtl8xxxu_write32(priv, REG_TX_AGC_B_RATE54_24,
			 ofdm_b + power_base->reg_0834);

	mcs_a = mcsbase[0] | mcsbase[0] << 8 |
		mcsbase[0] << 16 | mcsbase[0] << 24;
	mcs_b = mcsbase[1] | mcsbase[1] << 8 |
		mcsbase[1] << 16 | mcsbase[1] << 24;

	rtl8xxxu_write32(priv, REG_TX_AGC_A_MCS03_MCS00,
			 mcs_a + power_base->reg_0e10);
	rtl8xxxu_write32(priv, REG_TX_AGC_B_MCS03_MCS00,
			 mcs_b + power_base->reg_083c);

	rtl8xxxu_write32(priv, REG_TX_AGC_A_MCS07_MCS04,
			 mcs_a + power_base->reg_0e14);
	rtl8xxxu_write32(priv, REG_TX_AGC_B_MCS07_MCS04,
			 mcs_b + power_base->reg_0848);

	rtl8xxxu_write32(priv, REG_TX_AGC_A_MCS11_MCS08,
			 mcs_a + power_base->reg_0e18);
	rtl8xxxu_write32(priv, REG_TX_AGC_B_MCS11_MCS08,
			 mcs_b + power_base->reg_084c);

	rtl8xxxu_write32(priv, REG_TX_AGC_A_MCS15_MCS12,
			 mcs_a + power_base->reg_0e1c);
	for (i = 0; i < 3; i++) {
		if (i != 2)
			val8 = (mcsbase[0] > 8) ? (mcsbase[0] - 8) : 0;
		else
			val8 = (mcsbase[0] > 6) ? (mcsbase[0] - 6) : 0;
		rtl8xxxu_write8(priv, REG_OFDM0_XC_TX_IQ_IMBALANCE + i, val8);
	}
	rtl8xxxu_write32(priv, REG_TX_AGC_B_MCS15_MCS12,
			 mcs_b + power_base->reg_0868);
	for (i = 0; i < 3; i++) {
		if (i != 2)
			val8 = (mcsbase[1] > 8) ? (mcsbase[1] - 8) : 0;
		else
			val8 = (mcsbase[1] > 6) ? (mcsbase[1] - 6) : 0;
		rtl8xxxu_write8(priv, REG_OFDM0_XD_TX_IQ_IMBALANCE + i, val8);
	}
}