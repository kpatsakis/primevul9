static void wmi_evt_addba_rx_req(struct wil6210_priv *wil, int id, void *d,
				 int len)
{
	struct wmi_rcp_addba_req_event *evt = d;

	wil_addba_rx_request(wil, evt->cidxtid, evt->dialog_token,
			     evt->ba_param_set, evt->ba_timeout,
			     evt->ba_seq_ctrl);
}