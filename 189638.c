int wmi_set_channel(struct wil6210_priv *wil, int channel)
{
	struct wmi_set_pcp_channel_cmd cmd = {
		.channel = channel - 1,
	};

	return wmi_send(wil, WMI_SET_PCP_CHANNEL_CMDID, &cmd, sizeof(cmd));
}