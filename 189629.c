int wmi_set_ssid(struct wil6210_priv *wil, u8 ssid_len, const void *ssid)
{
	struct wmi_set_ssid_cmd cmd = {
		.ssid_len = cpu_to_le32(ssid_len),
	};

	if (ssid_len > sizeof(cmd.ssid))
		return -EINVAL;

	memcpy(cmd.ssid, ssid, ssid_len);

	return wmi_send(wil, WMI_SET_SSID_CMDID, &cmd, sizeof(cmd));
}