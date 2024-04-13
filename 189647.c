int wmi_echo(struct wil6210_priv *wil)
{
	struct wmi_echo_cmd cmd = {
		.value = cpu_to_le32(0x12345678),
	};

	return wmi_call(wil, WMI_ECHO_CMDID, &cmd, sizeof(cmd),
			WMI_ECHO_RSP_EVENTID, NULL, 0, 50);
}