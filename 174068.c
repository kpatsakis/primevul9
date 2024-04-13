mwifiex_is_bss_static_wep(struct mwifiex_private *priv,
			  struct mwifiex_bssdescriptor *bss_desc)
{
	if (priv->sec_info.wep_enabled && !priv->sec_info.wpa_enabled &&
	    !priv->sec_info.wpa2_enabled && bss_desc->privacy) {
		return true;
	}
	return false;
}