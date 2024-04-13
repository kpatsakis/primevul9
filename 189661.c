static void wmi_evt_tx_mgmt(struct wil6210_priv *wil, int id, void *d, int len)
{
	struct wmi_tx_mgmt_packet_event *data = d;
	struct ieee80211_mgmt *mgmt_frame =
			(struct ieee80211_mgmt *)data->payload;
	int flen = len - offsetof(struct wmi_tx_mgmt_packet_event, payload);

	wil_hex_dump_wmi("MGMT Tx ", DUMP_PREFIX_OFFSET, 16, 1, mgmt_frame,
			 flen, true);
}