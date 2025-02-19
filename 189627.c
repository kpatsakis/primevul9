static const char *eventid2name(u16 eventid)
{
	switch (eventid) {
	case WMI_NOTIFY_REQ_DONE_EVENTID:
		return "WMI_NOTIFY_REQ_DONE_EVENT";
	case WMI_DISCONNECT_EVENTID:
		return "WMI_DISCONNECT_EVENT";
	case WMI_SW_TX_COMPLETE_EVENTID:
		return "WMI_SW_TX_COMPLETE_EVENT";
	case WMI_GET_RF_SECTOR_PARAMS_DONE_EVENTID:
		return "WMI_GET_RF_SECTOR_PARAMS_DONE_EVENT";
	case WMI_SET_RF_SECTOR_PARAMS_DONE_EVENTID:
		return "WMI_SET_RF_SECTOR_PARAMS_DONE_EVENT";
	case WMI_GET_SELECTED_RF_SECTOR_INDEX_DONE_EVENTID:
		return "WMI_GET_SELECTED_RF_SECTOR_INDEX_DONE_EVENT";
	case WMI_SET_SELECTED_RF_SECTOR_INDEX_DONE_EVENTID:
		return "WMI_SET_SELECTED_RF_SECTOR_INDEX_DONE_EVENT";
	case WMI_BRP_SET_ANT_LIMIT_EVENTID:
		return "WMI_BRP_SET_ANT_LIMIT_EVENT";
	case WMI_FW_READY_EVENTID:
		return "WMI_FW_READY_EVENT";
	case WMI_TRAFFIC_RESUME_EVENTID:
		return "WMI_TRAFFIC_RESUME_EVENT";
	case WMI_TOF_GET_TX_RX_OFFSET_EVENTID:
		return "WMI_TOF_GET_TX_RX_OFFSET_EVENT";
	case WMI_TOF_SET_TX_RX_OFFSET_EVENTID:
		return "WMI_TOF_SET_TX_RX_OFFSET_EVENT";
	case WMI_VRING_CFG_DONE_EVENTID:
		return "WMI_VRING_CFG_DONE_EVENT";
	case WMI_READY_EVENTID:
		return "WMI_READY_EVENT";
	case WMI_RX_MGMT_PACKET_EVENTID:
		return "WMI_RX_MGMT_PACKET_EVENT";
	case WMI_TX_MGMT_PACKET_EVENTID:
		return "WMI_TX_MGMT_PACKET_EVENT";
	case WMI_SCAN_COMPLETE_EVENTID:
		return "WMI_SCAN_COMPLETE_EVENT";
	case WMI_ACS_PASSIVE_SCAN_COMPLETE_EVENTID:
		return "WMI_ACS_PASSIVE_SCAN_COMPLETE_EVENT";
	case WMI_CONNECT_EVENTID:
		return "WMI_CONNECT_EVENT";
	case WMI_EAPOL_RX_EVENTID:
		return "WMI_EAPOL_RX_EVENT";
	case WMI_BA_STATUS_EVENTID:
		return "WMI_BA_STATUS_EVENT";
	case WMI_RCP_ADDBA_REQ_EVENTID:
		return "WMI_RCP_ADDBA_REQ_EVENT";
	case WMI_DELBA_EVENTID:
		return "WMI_DELBA_EVENT";
	case WMI_VRING_EN_EVENTID:
		return "WMI_VRING_EN_EVENT";
	case WMI_DATA_PORT_OPEN_EVENTID:
		return "WMI_DATA_PORT_OPEN_EVENT";
	case WMI_AOA_MEAS_EVENTID:
		return "WMI_AOA_MEAS_EVENT";
	case WMI_TOF_SESSION_END_EVENTID:
		return "WMI_TOF_SESSION_END_EVENT";
	case WMI_TOF_GET_CAPABILITIES_EVENTID:
		return "WMI_TOF_GET_CAPABILITIES_EVENT";
	case WMI_TOF_SET_LCR_EVENTID:
		return "WMI_TOF_SET_LCR_EVENT";
	case WMI_TOF_SET_LCI_EVENTID:
		return "WMI_TOF_SET_LCI_EVENT";
	case WMI_TOF_FTM_PER_DEST_RES_EVENTID:
		return "WMI_TOF_FTM_PER_DEST_RES_EVENT";
	case WMI_TOF_CHANNEL_INFO_EVENTID:
		return "WMI_TOF_CHANNEL_INFO_EVENT";
	case WMI_TRAFFIC_SUSPEND_EVENTID:
		return "WMI_TRAFFIC_SUSPEND_EVENT";
	case WMI_ECHO_RSP_EVENTID:
		return "WMI_ECHO_RSP_EVENT";
	case WMI_LED_CFG_DONE_EVENTID:
		return "WMI_LED_CFG_DONE_EVENT";
	case WMI_PCP_STARTED_EVENTID:
		return "WMI_PCP_STARTED_EVENT";
	case WMI_PCP_STOPPED_EVENTID:
		return "WMI_PCP_STOPPED_EVENT";
	case WMI_GET_SSID_EVENTID:
		return "WMI_GET_SSID_EVENT";
	case WMI_GET_PCP_CHANNEL_EVENTID:
		return "WMI_GET_PCP_CHANNEL_EVENT";
	case WMI_P2P_CFG_DONE_EVENTID:
		return "WMI_P2P_CFG_DONE_EVENT";
	case WMI_LISTEN_STARTED_EVENTID:
		return "WMI_LISTEN_STARTED_EVENT";
	case WMI_SEARCH_STARTED_EVENTID:
		return "WMI_SEARCH_STARTED_EVENT";
	case WMI_DISCOVERY_STOPPED_EVENTID:
		return "WMI_DISCOVERY_STOPPED_EVENT";
	case WMI_CFG_RX_CHAIN_DONE_EVENTID:
		return "WMI_CFG_RX_CHAIN_DONE_EVENT";
	case WMI_TEMP_SENSE_DONE_EVENTID:
		return "WMI_TEMP_SENSE_DONE_EVENT";
	case WMI_RCP_ADDBA_RESP_SENT_EVENTID:
		return "WMI_RCP_ADDBA_RESP_SENT_EVENT";
	case WMI_PS_DEV_PROFILE_CFG_EVENTID:
		return "WMI_PS_DEV_PROFILE_CFG_EVENT";
	case WMI_SET_MGMT_RETRY_LIMIT_EVENTID:
		return "WMI_SET_MGMT_RETRY_LIMIT_EVENT";
	case WMI_GET_MGMT_RETRY_LIMIT_EVENTID:
		return "WMI_GET_MGMT_RETRY_LIMIT_EVENT";
	case WMI_SET_THERMAL_THROTTLING_CFG_EVENTID:
		return "WMI_SET_THERMAL_THROTTLING_CFG_EVENT";
	case WMI_GET_THERMAL_THROTTLING_CFG_EVENTID:
		return "WMI_GET_THERMAL_THROTTLING_CFG_EVENT";
	case WMI_LINK_MAINTAIN_CFG_WRITE_DONE_EVENTID:
		return "WMI_LINK_MAINTAIN_CFG_WRITE_DONE_EVENT";
	case WMI_LO_POWER_CALIB_FROM_OTP_EVENTID:
		return "WMI_LO_POWER_CALIB_FROM_OTP_EVENT";
	default:
		return "Untracked EVENT";
	}
}