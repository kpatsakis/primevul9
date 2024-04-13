int ath6kl_wmi_cancel_remain_on_chnl_cmd(struct wmi *wmi, u8 if_idx)
{
	ath6kl_dbg(ATH6KL_DBG_WMI, "cancel_remain_on_chnl_cmd\n");
	return ath6kl_wmi_simple_cmd(wmi, if_idx,
				     WMI_CANCEL_REMAIN_ON_CHNL_CMDID);
}