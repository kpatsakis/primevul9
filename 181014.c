int ath6kl_wmi_get_tx_pwr_cmd(struct wmi *wmi, u8 if_idx)
{
	return ath6kl_wmi_simple_cmd(wmi, if_idx, WMI_GET_TX_PWR_CMDID);
}