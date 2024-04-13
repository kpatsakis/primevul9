mwifiex_wmm_process_association_req(struct mwifiex_private *priv,
				    u8 **assoc_buf,
				    struct ieee_types_wmm_parameter *wmm_ie,
				    struct ieee80211_ht_cap *ht_cap)
{
	struct mwifiex_ie_types_wmm_param_set *wmm_tlv;
	u32 ret_len = 0;

	/* Null checks */
	if (!assoc_buf)
		return 0;
	if (!(*assoc_buf))
		return 0;

	if (!wmm_ie)
		return 0;

	mwifiex_dbg(priv->adapter, INFO,
		    "info: WMM: process assoc req: bss->wmm_ie=%#x\n",
		    wmm_ie->vend_hdr.element_id);

	if ((priv->wmm_required ||
	     (ht_cap && (priv->adapter->config_bands & BAND_GN ||
	     priv->adapter->config_bands & BAND_AN))) &&
	    wmm_ie->vend_hdr.element_id == WLAN_EID_VENDOR_SPECIFIC) {
		wmm_tlv = (struct mwifiex_ie_types_wmm_param_set *) *assoc_buf;
		wmm_tlv->header.type = cpu_to_le16((u16) wmm_info_ie[0]);
		wmm_tlv->header.len = cpu_to_le16((u16) wmm_info_ie[1]);
		memcpy(wmm_tlv->wmm_ie, &wmm_info_ie[2],
		       le16_to_cpu(wmm_tlv->header.len));
		if (wmm_ie->qos_info_bitmap & IEEE80211_WMM_IE_AP_QOSINFO_UAPSD)
			memcpy((u8 *) (wmm_tlv->wmm_ie
				       + le16_to_cpu(wmm_tlv->header.len)
				       - sizeof(priv->wmm_qosinfo)),
			       &priv->wmm_qosinfo, sizeof(priv->wmm_qosinfo));

		ret_len = sizeof(wmm_tlv->header)
			  + le16_to_cpu(wmm_tlv->header.len);

		*assoc_buf += ret_len;
	}

	return ret_len;
}