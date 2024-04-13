static void wmi_event_handle(struct wil6210_priv *wil,
			     struct wil6210_mbox_hdr *hdr)
{
	u16 len = le16_to_cpu(hdr->len);

	if ((hdr->type == WIL_MBOX_HDR_TYPE_WMI) &&
	    (len >= sizeof(struct wmi_cmd_hdr))) {
		struct wmi_cmd_hdr *wmi = (void *)(&hdr[1]);
		void *evt_data = (void *)(&wmi[1]);
		u16 id = le16_to_cpu(wmi->command_id);

		wil_dbg_wmi(wil, "Handle %s (0x%04x) (reply_id 0x%04x)\n",
			    eventid2name(id), id, wil->reply_id);
		/* check if someone waits for this event */
		if (wil->reply_id && wil->reply_id == id) {
			WARN_ON(wil->reply_buf);
			wmi_evt_call_handler(wil, id, evt_data,
					     len - sizeof(*wmi));
			wil_dbg_wmi(wil, "event_handle: Complete WMI 0x%04x\n",
				    id);
			complete(&wil->wmi_call);
			return;
		}
		/* unsolicited event */
		/* search for handler */
		if (!wmi_evt_call_handler(wil, id, evt_data,
					  len - sizeof(*wmi))) {
			wil_info(wil, "Unhandled event 0x%04x\n", id);
		}
	} else {
		wil_err(wil, "Unknown event type\n");
		print_hex_dump(KERN_ERR, "evt?? ", DUMP_PREFIX_OFFSET, 16, 1,
			       hdr, sizeof(*hdr) + len, true);
	}
}