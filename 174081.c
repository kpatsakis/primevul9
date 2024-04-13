_dbg_security_flags(int log_level, const char *func, const char *desc,
		    struct mwifiex_private *priv,
		    struct mwifiex_bssdescriptor *bss_desc)
{
	_mwifiex_dbg(priv->adapter, log_level,
		     "info: %s: %s:\twpa_ie=%#x wpa2_ie=%#x WEP=%s WPA=%s WPA2=%s\tEncMode=%#x privacy=%#x\n",
		     func, desc,
		     bss_desc->bcn_wpa_ie ?
		     bss_desc->bcn_wpa_ie->vend_hdr.element_id : 0,
		     bss_desc->bcn_rsn_ie ?
		     bss_desc->bcn_rsn_ie->ieee_hdr.element_id : 0,
		     priv->sec_info.wep_enabled ? "e" : "d",
		     priv->sec_info.wpa_enabled ? "e" : "d",
		     priv->sec_info.wpa2_enabled ? "e" : "d",
		     priv->sec_info.encryption_mode,
		     bss_desc->privacy);
}