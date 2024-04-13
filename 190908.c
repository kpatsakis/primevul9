   Set or fetch imap timeout */
PHP_FUNCTION(imap_timeout)
{
	zend_long ttype, timeout=-1;
	int timeout_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &ttype, &timeout) == FAILURE) {
		RETURN_FALSE;
	}

	if (timeout == -1) {
		switch (ttype) {
			case 1:
				timeout_type = GET_OPENTIMEOUT;
				break;
			case 2:
				timeout_type = GET_READTIMEOUT;
				break;
			case 3:
				timeout_type = GET_WRITETIMEOUT;
				break;
			case 4:
				timeout_type = GET_CLOSETIMEOUT;
				break;
			default:
				RETURN_FALSE;
				break;
		}

		timeout = (zend_long) mail_parameters(NIL, timeout_type, NIL);
		RETURN_LONG(timeout);
	} else if (timeout >= 0) {
		switch (ttype) {
			case 1:
				timeout_type = SET_OPENTIMEOUT;
				break;
			case 2:
				timeout_type = SET_READTIMEOUT;
				break;
			case 3:
				timeout_type = SET_WRITETIMEOUT;
				break;
			case 4:
				timeout_type = SET_CLOSETIMEOUT;
				break;
			default:
				RETURN_FALSE;
				break;
		}

		timeout = (zend_long) mail_parameters(NIL, timeout_type, (void *) timeout);
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}