void rtl8xxxu_gen2_config_channel(struct ieee80211_hw *hw)
{
	struct rtl8xxxu_priv *priv = hw->priv;
	u32 val32, rsr;
	u8 val8, subchannel;
	u16 rf_mode_bw;
	bool ht = true;
	int sec_ch_above, channel;
	int i;

	rf_mode_bw = rtl8xxxu_read16(priv, REG_WMAC_TRXPTCL_CTL);
	rf_mode_bw &= ~WMAC_TRXPTCL_CTL_BW_MASK;
	rsr = rtl8xxxu_read32(priv, REG_RESPONSE_RATE_SET);
	channel = hw->conf.chandef.chan->hw_value;

/* Hack */
	subchannel = 0;

	switch (hw->conf.chandef.width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
		ht = false;
		/* fall through */
	case NL80211_CHAN_WIDTH_20:
		rf_mode_bw |= WMAC_TRXPTCL_CTL_BW_20;
		subchannel = 0;

		val32 = rtl8xxxu_read32(priv, REG_FPGA0_RF_MODE);
		val32 &= ~FPGA_RF_MODE;
		rtl8xxxu_write32(priv, REG_FPGA0_RF_MODE, val32);

		val32 = rtl8xxxu_read32(priv, REG_FPGA1_RF_MODE);
		val32 &= ~FPGA_RF_MODE;
		rtl8xxxu_write32(priv, REG_FPGA1_RF_MODE, val32);

		val32 = rtl8xxxu_read32(priv, REG_OFDM0_TX_PSDO_NOISE_WEIGHT);
		val32 &= ~(BIT(30) | BIT(31));
		rtl8xxxu_write32(priv, REG_OFDM0_TX_PSDO_NOISE_WEIGHT, val32);

		break;
	case NL80211_CHAN_WIDTH_40:
		rf_mode_bw |= WMAC_TRXPTCL_CTL_BW_40;

		if (hw->conf.chandef.center_freq1 >
		    hw->conf.chandef.chan->center_freq) {
			sec_ch_above = 1;
			channel += 2;
		} else {
			sec_ch_above = 0;
			channel -= 2;
		}

		val32 = rtl8xxxu_read32(priv, REG_FPGA0_RF_MODE);
		val32 |= FPGA_RF_MODE;
		rtl8xxxu_write32(priv, REG_FPGA0_RF_MODE, val32);

		val32 = rtl8xxxu_read32(priv, REG_FPGA1_RF_MODE);
		val32 |= FPGA_RF_MODE;
		rtl8xxxu_write32(priv, REG_FPGA1_RF_MODE, val32);

		/*
		 * Set Control channel to upper or lower. These settings
		 * are required only for 40MHz
		 */
		val32 = rtl8xxxu_read32(priv, REG_CCK0_SYSTEM);
		val32 &= ~CCK0_SIDEBAND;
		if (!sec_ch_above)
			val32 |= CCK0_SIDEBAND;
		rtl8xxxu_write32(priv, REG_CCK0_SYSTEM, val32);

		val32 = rtl8xxxu_read32(priv, REG_OFDM1_LSTF);
		val32 &= ~OFDM_LSTF_PRIME_CH_MASK; /* 0xc00 */
		if (sec_ch_above)
			val32 |= OFDM_LSTF_PRIME_CH_LOW;
		else
			val32 |= OFDM_LSTF_PRIME_CH_HIGH;
		rtl8xxxu_write32(priv, REG_OFDM1_LSTF, val32);

		val32 = rtl8xxxu_read32(priv, REG_FPGA0_POWER_SAVE);
		val32 &= ~(FPGA0_PS_LOWER_CHANNEL | FPGA0_PS_UPPER_CHANNEL);
		if (sec_ch_above)
			val32 |= FPGA0_PS_UPPER_CHANNEL;
		else
			val32 |= FPGA0_PS_LOWER_CHANNEL;
		rtl8xxxu_write32(priv, REG_FPGA0_POWER_SAVE, val32);
		break;
	case NL80211_CHAN_WIDTH_80:
		rf_mode_bw |= WMAC_TRXPTCL_CTL_BW_80;
		break;
	default:
		break;
	}

	for (i = RF_A; i < priv->rf_paths; i++) {
		val32 = rtl8xxxu_read_rfreg(priv, i, RF6052_REG_MODE_AG);
		val32 &= ~MODE_AG_CHANNEL_MASK;
		val32 |= channel;
		rtl8xxxu_write_rfreg(priv, i, RF6052_REG_MODE_AG, val32);
	}

	rtl8xxxu_write16(priv, REG_WMAC_TRXPTCL_CTL, rf_mode_bw);
	rtl8xxxu_write8(priv, REG_DATA_SUBCHANNEL, subchannel);

	if (ht)
		val8 = 0x0e;
	else
		val8 = 0x0a;

	rtl8xxxu_write8(priv, REG_SIFS_CCK + 1, val8);
	rtl8xxxu_write8(priv, REG_SIFS_OFDM + 1, val8);

	rtl8xxxu_write16(priv, REG_R2T_SIFS, 0x0808);
	rtl8xxxu_write16(priv, REG_T2T_SIFS, 0x0a0a);

	for (i = RF_A; i < priv->rf_paths; i++) {
		val32 = rtl8xxxu_read_rfreg(priv, i, RF6052_REG_MODE_AG);
		val32 &= ~MODE_AG_BW_MASK;
		switch(hw->conf.chandef.width) {
		case NL80211_CHAN_WIDTH_80:
			val32 |= MODE_AG_BW_80MHZ_8723B;
			break;
		case NL80211_CHAN_WIDTH_40:
			val32 |= MODE_AG_BW_40MHZ_8723B;
			break;
		default:
			val32 |= MODE_AG_BW_20MHZ_8723B;
			break;
		}
		rtl8xxxu_write_rfreg(priv, i, RF6052_REG_MODE_AG, val32);
	}
}