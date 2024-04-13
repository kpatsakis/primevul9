static int muscle_pin_cmd(sc_card_t *card, struct sc_pin_cmd_data *cmd,
				int *tries_left)
{
	muscle_private_t* priv = MUSCLE_DATA(card);
	const int bufferLength = MSC_MAX_PIN_COMMAND_LENGTH;
	u8 buffer[MSC_MAX_PIN_COMMAND_LENGTH];
	switch(cmd->cmd) {
	case SC_PIN_CMD_VERIFY:
		switch(cmd->pin_type) {
		case SC_AC_CHV: {
			sc_apdu_t apdu;
			int r;
			msc_verify_pin_apdu(card, &apdu, buffer, bufferLength, cmd->pin_reference, cmd->pin1.data, cmd->pin1.len);
			cmd->apdu = &apdu;
			cmd->pin1.offset = 5;
			r = iso_ops->pin_cmd(card, cmd, tries_left);
			if(r >= 0)
				priv->verifiedPins |= (1 << cmd->pin_reference);
			return r;
		}
		case SC_AC_TERM:
		case SC_AC_PRO:
		case SC_AC_AUT:
		case SC_AC_NONE:
		default:
			sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL, "Unsupported authentication method\n");
			return SC_ERROR_NOT_SUPPORTED;
		}
	case SC_PIN_CMD_CHANGE:
		switch(cmd->pin_type) {
		case SC_AC_CHV: {
			sc_apdu_t apdu;
			msc_change_pin_apdu(card, &apdu, buffer, bufferLength, cmd->pin_reference, cmd->pin1.data, cmd->pin1.len, cmd->pin2.data, cmd->pin2.len);
			cmd->apdu = &apdu;
			return iso_ops->pin_cmd(card, cmd, tries_left);
		}
		case SC_AC_TERM:
		case SC_AC_PRO:
		case SC_AC_AUT:
		case SC_AC_NONE:
		default:
			sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL, "Unsupported authentication method\n");
			return SC_ERROR_NOT_SUPPORTED;
		}
	case SC_PIN_CMD_UNBLOCK:
	switch(cmd->pin_type) {
		case SC_AC_CHV: {
			sc_apdu_t apdu;
			msc_unblock_pin_apdu(card, &apdu, buffer, bufferLength, cmd->pin_reference, cmd->pin1.data, cmd->pin1.len);
			cmd->apdu = &apdu;
			return iso_ops->pin_cmd(card, cmd, tries_left);
		}
		case SC_AC_TERM:
		case SC_AC_PRO:
		case SC_AC_AUT:
		case SC_AC_NONE:
		default:
			sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL, "Unsupported authentication method\n");
			return SC_ERROR_NOT_SUPPORTED;
		}
	default:
		sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL, "Unsupported command\n");
		return SC_ERROR_NOT_SUPPORTED;

	}

}