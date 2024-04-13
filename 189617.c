static void wmi_evt_ba_status(struct wil6210_priv *wil, int id, void *d,
			      int len)
{
	struct wmi_ba_status_event *evt = d;
	struct vring_tx_data *txdata;

	wil_dbg_wmi(wil, "BACK[%d] %s {%d} timeout %d AMSDU%s\n",
		    evt->ringid,
		    evt->status == WMI_BA_AGREED ? "OK" : "N/A",
		    evt->agg_wsize, __le16_to_cpu(evt->ba_timeout),
		    evt->amsdu ? "+" : "-");

	if (evt->ringid >= WIL6210_MAX_TX_RINGS) {
		wil_err(wil, "invalid ring id %d\n", evt->ringid);
		return;
	}

	if (evt->status != WMI_BA_AGREED) {
		evt->ba_timeout = 0;
		evt->agg_wsize = 0;
		evt->amsdu = 0;
	}

	txdata = &wil->vring_tx_data[evt->ringid];

	txdata->agg_timeout = le16_to_cpu(evt->ba_timeout);
	txdata->agg_wsize = evt->agg_wsize;
	txdata->agg_amsdu = evt->amsdu;
	txdata->addba_in_progress = false;
}