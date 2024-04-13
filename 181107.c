static int ath6kl_wmi_roam_tbl_event_rx(struct wmi *wmi, u8 *datap, int len)
{
	return ath6kl_debug_roam_tbl_event(wmi->parent_dev, datap, len);
}