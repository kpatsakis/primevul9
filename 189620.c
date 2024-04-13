int wmi_new_sta(struct wil6210_priv *wil, const u8 *mac, u8 aid)
{
	int rc;
	struct wmi_new_sta_cmd cmd = {
		.aid = aid,
	};

	wil_dbg_wmi(wil, "new sta %pM, aid %d\n", mac, aid);

	ether_addr_copy(cmd.dst_mac, mac);

	rc = wmi_send(wil, WMI_NEW_STA_CMDID, &cmd, sizeof(cmd));
	if (rc)
		wil_err(wil, "Failed to send new sta (%d)\n", rc);

	return rc;
}