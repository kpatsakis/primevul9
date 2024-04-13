smtp_message_end(struct smtp_tx *tx)
{
	struct smtp_session *s;

	s = tx->session;

	log_debug("debug: %p: end of message, error=%d", s, tx->error);

	fclose(tx->ofile);
	tx->ofile = NULL;

	switch(tx->error) {
	case TX_OK:
		smtp_tx_commit(tx);
		return;		

	case TX_ERROR_SIZE:
		smtp_reply(s, "554 %s %s: Transaction failed, message too big",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_MESSAGE_TOO_BIG_FOR_SYSTEM),
		    esc_description(ESC_MESSAGE_TOO_BIG_FOR_SYSTEM));
		break;

	case TX_ERROR_LOOP:
		smtp_reply(s, "500 %s %s: Loop detected",
		   esc_code(ESC_STATUS_PERMFAIL, ESC_ROUTING_LOOP_DETECTED),
		   esc_description(ESC_ROUTING_LOOP_DETECTED));
		break;

	case TX_ERROR_MALFORMED:
		smtp_reply(s, "550 %s %s: Message is not RFC 2822 compliant",
		    esc_code(ESC_STATUS_PERMFAIL, ESC_DELIVERY_NOT_AUTHORIZED_MESSAGE_REFUSED),
		    esc_description(ESC_DELIVERY_NOT_AUTHORIZED_MESSAGE_REFUSED));
		break;

	case TX_ERROR_IO:
	case TX_ERROR_RESOURCES:
		smtp_reply(s, "421 %s Temporary Error",
		    esc_code(ESC_STATUS_TEMPFAIL, ESC_OTHER_MAIL_SYSTEM_STATUS));
		break;

	default:
		/* fatal? */
		smtp_reply(s, "421 Internal server error");
	}

	smtp_tx_rollback(tx);
	smtp_tx_free(tx);
	smtp_enter_state(s, STATE_HELO);
}