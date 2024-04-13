mwifiex_is_bss_dynamic_wep(struct mwifiex_private *priv,
			   struct mwifiex_bssdescriptor *bss_desc)
{
	if (!priv->sec_info.wep_enabled && !priv->sec_info.wpa_enabled &&
	    !priv->sec_info.wpa2_enabled &&
	    !has_vendor_hdr(bss_desc->bcn_wpa_ie, WLAN_EID_VENDOR_SPECIFIC) &&
	    !has_ieee_hdr(bss_desc->bcn_rsn_ie, WLAN_EID_RSN) &&
	    priv->sec_info.encryption_mode && bss_desc->privacy) {
		dbg_security_flags(INFO, "dynamic", priv, bss_desc);
		return true;
	}
	return false;
}