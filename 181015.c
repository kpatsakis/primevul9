int ath6kl_wmi_get_roam_tbl_cmd(struct wmi *wmi)
{
	return ath6kl_wmi_simple_cmd(wmi, 0, WMI_GET_ROAM_TBL_CMDID);
}