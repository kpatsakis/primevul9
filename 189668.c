int wmi_addba(struct wil6210_priv *wil, u8 ringid, u8 size, u16 timeout)
{
	struct wmi_vring_ba_en_cmd cmd = {
		.ringid = ringid,
		.agg_max_wsize = size,
		.ba_timeout = cpu_to_le16(timeout),
		.amsdu = 0,
	};

	wil_dbg_wmi(wil, "addba: (ring %d size %d timeout %d)\n", ringid, size,
		    timeout);

	return wmi_send(wil, WMI_VRING_BA_EN_CMDID, &cmd, sizeof(cmd));
}