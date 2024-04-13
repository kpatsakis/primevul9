mwifiex_is_bss_wapi(struct mwifiex_private *priv,
		    struct mwifiex_bssdescriptor *bss_desc)
{
	if (priv->sec_info.wapi_enabled &&
	    has_ieee_hdr(bss_desc->bcn_wapi_ie, WLAN_EID_BSS_AC_ACCESS_DELAY))
		return true;
	return false;
}