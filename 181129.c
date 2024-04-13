static int ath6kl_wmi_tx_complete_event_rx(u8 *datap, int len)
{
	struct tx_complete_msg_v1 *msg_v1;
	struct wmi_tx_complete_event *evt;
	int index;
	u16 size;

	evt = (struct wmi_tx_complete_event *) datap;

	ath6kl_dbg(ATH6KL_DBG_WMI, "comp: %d %d %d\n",
		   evt->num_msg, evt->msg_len, evt->msg_type);

	for (index = 0; index < evt->num_msg; index++) {
		size = sizeof(struct wmi_tx_complete_event) +
		    (index * sizeof(struct tx_complete_msg_v1));
		msg_v1 = (struct tx_complete_msg_v1 *)(datap + size);

		ath6kl_dbg(ATH6KL_DBG_WMI, "msg: %d %d %d %d\n",
			   msg_v1->status, msg_v1->pkt_id,
			   msg_v1->rate_idx, msg_v1->ack_failures);
	}

	return 0;
}