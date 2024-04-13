static void rtl8xxxu_rx_parse_phystats(struct rtl8xxxu_priv *priv,
				       struct ieee80211_rx_status *rx_status,
				       struct rtl8723au_phy_stats *phy_stats,
				       u32 rxmcs)
{
	if (phy_stats->sgi_en)
		rx_status->enc_flags |= RX_ENC_FLAG_SHORT_GI;

	if (rxmcs < DESC_RATE_6M) {
		/*
		 * Handle PHY stats for CCK rates
		 */
		u8 cck_agc_rpt = phy_stats->cck_agc_rpt_ofdm_cfosho_a;

		switch (cck_agc_rpt & 0xc0) {
		case 0xc0:
			rx_status->signal = -46 - (cck_agc_rpt & 0x3e);
			break;
		case 0x80:
			rx_status->signal = -26 - (cck_agc_rpt & 0x3e);
			break;
		case 0x40:
			rx_status->signal = -12 - (cck_agc_rpt & 0x3e);
			break;
		case 0x00:
			rx_status->signal = 16 - (cck_agc_rpt & 0x3e);
			break;
		}
	} else {
		rx_status->signal =
			(phy_stats->cck_sig_qual_ofdm_pwdb_all >> 1) - 110;
	}
}