static int ath6kl_wmi_aplist_event_rx(struct wmi *wmi, u8 *datap, int len)
{
	u16 ap_info_entry_size;
	struct wmi_aplist_event *ev = (struct wmi_aplist_event *) datap;
	struct wmi_ap_info_v1 *ap_info_v1;
	u8 index;

	if (len < sizeof(struct wmi_aplist_event) ||
	    ev->ap_list_ver != APLIST_VER1)
		return -EINVAL;

	ap_info_entry_size = sizeof(struct wmi_ap_info_v1);
	ap_info_v1 = (struct wmi_ap_info_v1 *) ev->ap_list;

	ath6kl_dbg(ATH6KL_DBG_WMI,
		   "number of APs in aplist event: %d\n", ev->num_ap);

	if (len < (int) (sizeof(struct wmi_aplist_event) +
			 (ev->num_ap - 1) * ap_info_entry_size))
		return -EINVAL;

	/* AP list version 1 contents */
	for (index = 0; index < ev->num_ap; index++) {
		ath6kl_dbg(ATH6KL_DBG_WMI, "AP#%d BSSID %pM Channel %d\n",
			   index, ap_info_v1->bssid, ap_info_v1->channel);
		ap_info_v1++;
	}

	return 0;
}