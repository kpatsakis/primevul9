mwifiex_append_rate_tlv(struct mwifiex_private *priv,
			struct mwifiex_scan_cmd_config *scan_cfg_out,
			u8 radio)
{
	struct mwifiex_ie_types_rates_param_set *rates_tlv;
	u8 rates[MWIFIEX_SUPPORTED_RATES], *tlv_pos;
	u32 rates_size;

	memset(rates, 0, sizeof(rates));

	tlv_pos = (u8 *)scan_cfg_out->tlv_buf + scan_cfg_out->tlv_buf_len;

	if (priv->scan_request)
		rates_size = mwifiex_get_rates_from_cfg80211(priv, rates,
							     radio);
	else
		rates_size = mwifiex_get_supported_rates(priv, rates);

	mwifiex_dbg(priv->adapter, CMD,
		    "info: SCAN_CMD: Rates size = %d\n",
		rates_size);
	rates_tlv = (struct mwifiex_ie_types_rates_param_set *)tlv_pos;
	rates_tlv->header.type = cpu_to_le16(WLAN_EID_SUPP_RATES);
	rates_tlv->header.len = cpu_to_le16((u16) rates_size);
	memcpy(rates_tlv->rates, rates, rates_size);
	scan_cfg_out->tlv_buf_len += sizeof(rates_tlv->header) + rates_size;

	return rates_size;
}