int wmi_get_channel(struct wil6210_priv *wil, int *channel)
{
	int rc;
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_set_pcp_channel_cmd cmd;
	} __packed reply;

	rc = wmi_call(wil, WMI_GET_PCP_CHANNEL_CMDID, NULL, 0,
		      WMI_GET_PCP_CHANNEL_EVENTID, &reply, sizeof(reply), 20);
	if (rc)
		return rc;

	if (reply.cmd.channel > 3)
		return -EINVAL;

	*channel = reply.cmd.channel + 1;

	return 0;
}