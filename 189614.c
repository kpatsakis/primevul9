int wmi_disconnect_sta(struct wil6210_priv *wil, const u8 *mac,
		       u16 reason, bool full_disconnect, bool del_sta)
{
	int rc;
	u16 reason_code;
	struct wmi_disconnect_sta_cmd disc_sta_cmd = {
		.disconnect_reason = cpu_to_le16(reason),
	};
	struct wmi_del_sta_cmd del_sta_cmd = {
		.disconnect_reason = cpu_to_le16(reason),
	};
	struct {
		struct wmi_cmd_hdr wmi;
		struct wmi_disconnect_event evt;
	} __packed reply;

	wil_dbg_wmi(wil, "disconnect_sta: (%pM, reason %d)\n", mac, reason);

	wil->locally_generated_disc = true;
	if (del_sta) {
		ether_addr_copy(del_sta_cmd.dst_mac, mac);
		rc = wmi_call(wil, WMI_DEL_STA_CMDID, &del_sta_cmd,
			      sizeof(del_sta_cmd), WMI_DISCONNECT_EVENTID,
			      &reply, sizeof(reply), 1000);
	} else {
		ether_addr_copy(disc_sta_cmd.dst_mac, mac);
		rc = wmi_call(wil, WMI_DISCONNECT_STA_CMDID, &disc_sta_cmd,
			      sizeof(disc_sta_cmd), WMI_DISCONNECT_EVENTID,
			      &reply, sizeof(reply), 1000);
	}
	/* failure to disconnect in reasonable time treated as FW error */
	if (rc) {
		wil_fw_error_recovery(wil);
		return rc;
	}

	if (full_disconnect) {
		/* call event handler manually after processing wmi_call,
		 * to avoid deadlock - disconnect event handler acquires
		 * wil->mutex while it is already held here
		 */
		reason_code = le16_to_cpu(reply.evt.protocol_reason_status);

		wil_dbg_wmi(wil, "Disconnect %pM reason [proto %d wmi %d]\n",
			    reply.evt.bssid, reason_code,
			    reply.evt.disconnect_reason);

		wil->sinfo_gen++;
		wil6210_disconnect(wil, reply.evt.bssid, reason_code, true);
	}
	return 0;
}