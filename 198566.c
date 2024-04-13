static int muscle_check_sw(sc_card_t * card, unsigned int sw1, unsigned int sw2) {
	if(sw1 == 0x9C) {
		switch(sw2) {
			case 0x01: /* SW_NO_MEMORY_LEFT */
				return SC_ERROR_NOT_ENOUGH_MEMORY;
			case 0x02: /* SW_AUTH_FAILED */
				return SC_ERROR_PIN_CODE_INCORRECT;
			case 0x03: /* SW_OPERATION_NOT_ALLOWED */
				return SC_ERROR_NOT_ALLOWED;
			case 0x05: /* SW_UNSUPPORTED_FEATURE */
				return SC_ERROR_NO_CARD_SUPPORT;
			case 0x06: /* SW_UNAUTHORIZED */
				return SC_ERROR_SECURITY_STATUS_NOT_SATISFIED;
			case 0x07: /* SW_OBJECT_NOT_FOUND */
				return SC_ERROR_FILE_NOT_FOUND;
			case 0x08: /* SW_OBJECT_EXISTS */
				return SC_ERROR_FILE_ALREADY_EXISTS;
			case 0x09: /* SW_INCORRECT_ALG */
				return SC_ERROR_INCORRECT_PARAMETERS;
			case 0x0B: /* SW_SIGNATURE_INVALID */
				return SC_ERROR_CARD_CMD_FAILED;
			case 0x0C: /* SW_IDENTITY_BLOCKED */
				return SC_ERROR_AUTH_METHOD_BLOCKED;
			case 0x0F: /* SW_INVALID_PARAMETER */
			case 0x10: /* SW_INCORRECT_P1 */
			case 0x11: /* SW_INCORRECT_P2 */
				return SC_ERROR_INCORRECT_PARAMETERS;
		}
	}
	return iso_ops->check_sw(card, sw1, sw2);
}