int wmi_get_ssid(struct wil6210_priv *wil, u8 *ssid_len, void *ssid)
{
	int rc;
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_set_ssid_cmd cmd;
	} __packed reply;
	int len; /* reply.cmd.ssid_len in CPU order */

	rc = wmi_call(wil, WMI_GET_SSID_CMDID, NULL, 0, WMI_GET_SSID_EVENTID,
		      &reply, sizeof(reply), 20);
	if (rc)
		return rc;

	len = le32_to_cpu(reply.cmd.ssid_len);
	if (len > sizeof(reply.cmd.ssid))
		return -EINVAL;

	*ssid_len = len;
	memcpy(ssid, reply.cmd.ssid, len);

	return 0;
}