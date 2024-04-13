mwifiex_is_bss_wpa(struct mwifiex_private *priv,
		   struct mwifiex_bssdescriptor *bss_desc)
{
	if (!priv->sec_info.wep_enabled && priv->sec_info.wpa_enabled &&
	    !priv->sec_info.wpa2_enabled &&
	    has_vendor_hdr(bss_desc->bcn_wpa_ie, WLAN_EID_VENDOR_SPECIFIC)
	   /*
	    * Privacy bit may NOT be set in some APs like
	    * LinkSys WRT54G && bss_desc->privacy
	    */
	 ) {
		dbg_security_flags(INFO, "WPA", priv, bss_desc);
		return true;
	}
	return false;
}