int ath6kl_wmi_get_stats_cmd(struct wmi *wmi, u8 if_idx)
{
	return ath6kl_wmi_simple_cmd(wmi, if_idx, WMI_GET_STATISTICS_CMDID);
}