static int ath6kl_wmi_proc_events_vif(struct wmi *wmi, u16 if_idx, u16 cmd_id,
					u8 *datap, u32 len)
{
	struct ath6kl_vif *vif;

	vif = ath6kl_get_vif_by_index(wmi->parent_dev, if_idx);
	if (!vif) {
		ath6kl_dbg(ATH6KL_DBG_WMI,
			   "Wmi event for unavailable vif, vif_index:%d\n",
			    if_idx);
		return -EINVAL;
	}

	switch (cmd_id) {
	case WMI_CONNECT_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_CONNECT_EVENTID\n");
		return ath6kl_wmi_connect_event_rx(wmi, datap, len, vif);
	case WMI_DISCONNECT_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_DISCONNECT_EVENTID\n");
		return ath6kl_wmi_disconnect_event_rx(wmi, datap, len, vif);
	case WMI_TKIP_MICERR_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_TKIP_MICERR_EVENTID\n");
		return ath6kl_wmi_tkip_micerr_event_rx(wmi, datap, len, vif);
	case WMI_BSSINFO_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_BSSINFO_EVENTID\n");
		return ath6kl_wmi_bssinfo_event_rx(wmi, datap, len, vif);
	case WMI_NEIGHBOR_REPORT_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_NEIGHBOR_REPORT_EVENTID\n");
		return ath6kl_wmi_neighbor_report_event_rx(wmi, datap, len,
							   vif);
	case WMI_SCAN_COMPLETE_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_SCAN_COMPLETE_EVENTID\n");
		return ath6kl_wmi_scan_complete_rx(wmi, datap, len, vif);
	case WMI_REPORT_STATISTICS_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_REPORT_STATISTICS_EVENTID\n");
		return ath6kl_wmi_stats_event_rx(wmi, datap, len, vif);
	case WMI_CAC_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_CAC_EVENTID\n");
		return ath6kl_wmi_cac_event_rx(wmi, datap, len, vif);
	case WMI_PSPOLL_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_PSPOLL_EVENTID\n");
		return ath6kl_wmi_pspoll_event_rx(wmi, datap, len, vif);
	case WMI_DTIMEXPIRY_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_DTIMEXPIRY_EVENTID\n");
		return ath6kl_wmi_dtimexpiry_event_rx(wmi, datap, len, vif);
	case WMI_ADDBA_REQ_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_ADDBA_REQ_EVENTID\n");
		return ath6kl_wmi_addba_req_event_rx(wmi, datap, len, vif);
	case WMI_DELBA_REQ_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_DELBA_REQ_EVENTID\n");
		return ath6kl_wmi_delba_req_event_rx(wmi, datap, len, vif);
	case WMI_SET_HOST_SLEEP_MODE_CMD_PROCESSED_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI,
			   "WMI_SET_HOST_SLEEP_MODE_CMD_PROCESSED_EVENTID");
		return ath6kl_wmi_host_sleep_mode_cmd_prcd_evt_rx(wmi, vif);
	case WMI_REMAIN_ON_CHNL_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_REMAIN_ON_CHNL_EVENTID\n");
		return ath6kl_wmi_remain_on_chnl_event_rx(wmi, datap, len, vif);
	case WMI_CANCEL_REMAIN_ON_CHNL_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI,
			   "WMI_CANCEL_REMAIN_ON_CHNL_EVENTID\n");
		return ath6kl_wmi_cancel_remain_on_chnl_event_rx(wmi, datap,
								 len, vif);
	case WMI_TX_STATUS_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_TX_STATUS_EVENTID\n");
		return ath6kl_wmi_tx_status_event_rx(wmi, datap, len, vif);
	case WMI_RX_PROBE_REQ_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_RX_PROBE_REQ_EVENTID\n");
		return ath6kl_wmi_rx_probe_req_event_rx(wmi, datap, len, vif);
	case WMI_RX_ACTION_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_RX_ACTION_EVENTID\n");
		return ath6kl_wmi_rx_action_event_rx(wmi, datap, len, vif);
	case WMI_TXE_NOTIFY_EVENTID:
		ath6kl_dbg(ATH6KL_DBG_WMI, "WMI_TXE_NOTIFY_EVENTID\n");
		return ath6kl_wmi_txe_notify_event_rx(wmi, datap, len, vif);
	default:
		ath6kl_dbg(ATH6KL_DBG_WMI, "unknown cmd id 0x%x\n", cmd_id);
		return -EINVAL;
	}

	return 0;
}