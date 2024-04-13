int wmi_delba_rx(struct wil6210_priv *wil, u8 cidxtid, u16 reason)
{
	struct wmi_rcp_delba_cmd cmd = {
		.cidxtid = cidxtid,
		.reason = cpu_to_le16(reason),
	};

	wil_dbg_wmi(wil, "delba_rx: (CID %d TID %d reason %d)\n", cidxtid & 0xf,
		    (cidxtid >> 4) & 0xf, reason);

	return wmi_send(wil, WMI_RCP_DELBA_CMDID, &cmd, sizeof(cmd));
}