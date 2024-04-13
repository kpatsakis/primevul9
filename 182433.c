smb2_echo_callback(struct mid_q_entry *mid)
{
	struct TCP_Server_Info *server = mid->callback_data;
	struct smb2_echo_rsp *rsp = (struct smb2_echo_rsp *)mid->resp_buf;
	struct cifs_credits credits = { .value = 0, .instance = 0 };

	if (mid->mid_state == MID_RESPONSE_RECEIVED
	    || mid->mid_state == MID_RESPONSE_MALFORMED) {
		credits.value = le16_to_cpu(rsp->sync_hdr.CreditRequest);
		credits.instance = server->reconnect_instance;
	}

	DeleteMidQEntry(mid);
	add_credits(server, &credits, CIFS_ECHO_OP);
}