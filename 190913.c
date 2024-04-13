   Returns a properly formatted email address given the mailbox, host, and personal info */
PHP_FUNCTION(imap_rfc822_write_address)
{
	zend_string *mailbox, *host, *personal;
	ADDRESS *addr;
	zend_string *string;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SSS", &mailbox, &host, &personal) == FAILURE) {
		return;
	}

	addr=mail_newaddr();

	if (mailbox) {
		addr->mailbox = cpystr(ZSTR_VAL(mailbox));
	}

	if (host) {
		addr->host = cpystr(ZSTR_VAL(host));
	}

	if (personal) {
		addr->personal = cpystr(ZSTR_VAL(personal));
	}

	addr->next=NIL;
	addr->error=NIL;
	addr->adl=NIL;

	string = _php_rfc822_write_address(addr);
	if (string) {
		RETVAL_STR(string);
	} else {
		RETURN_FALSE;
	}