mwifiex_is_wpa_oui_present(struct mwifiex_bssdescriptor *bss_desc, u32 cipher)
{
	u8 *oui;
	struct ie_body *iebody;
	u8 ret = MWIFIEX_OUI_NOT_PRESENT;

	if (has_vendor_hdr(bss_desc->bcn_wpa_ie, WLAN_EID_VENDOR_SPECIFIC)) {
		iebody = (struct ie_body *)((u8 *)bss_desc->bcn_wpa_ie->data +
					    WPA_GTK_OUI_OFFSET);
		oui = &mwifiex_wpa_oui[cipher][0];
		ret = mwifiex_search_oui_in_ie(iebody, oui);
		if (ret)
			return ret;
	}
	return ret;
}