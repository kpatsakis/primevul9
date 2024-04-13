static NTSTATUS send_break_message(struct messaging_context *msg_ctx,
				   const struct share_mode_entry *exclusive,
				   uint16_t break_to)
{
	NTSTATUS status;
	char msg[MSG_SMB_SHARE_MODE_ENTRY_SIZE];

	DEBUG(10, ("Sending break request to PID %s\n",
		   procid_str_static(&exclusive->pid)));

	/* Create the message. */
	share_mode_entry_to_message(msg, exclusive);

	/* Overload entry->op_type */
	SSVAL(msg,OP_BREAK_MSG_OP_TYPE_OFFSET, break_to);

	status = messaging_send_buf(msg_ctx, exclusive->pid,
				    MSG_SMB_BREAK_REQUEST,
				    (uint8 *)msg, sizeof(msg));
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(3, ("Could not send oplock break message: %s\n",
			  nt_errstr(status)));
	}

	return status;
}