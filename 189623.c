int wmi_delba_tx(struct wil6210_priv *wil, u8 ringid, u16 reason)
{
	struct wmi_vring_ba_dis_cmd cmd = {
		.ringid = ringid,
		.reason = cpu_to_le16(reason),
	};

	wil_dbg_wmi(wil, "delba_tx: (ring %d reason %d)\n", ringid, reason);

	return wmi_send(wil, WMI_VRING_BA_DIS_CMDID, &cmd, sizeof(cmd));
}