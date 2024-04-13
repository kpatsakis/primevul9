   Returns a properly formatted email address given the mailbox, host, and personal info */
PHP_FUNCTION(imap_rfc822_write_address)
{
	char *mailbox, *host, *personal;
	int mailbox_len, host_len, personal_len;
	ADDRESS *addr;
	char *string;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &mailbox, &mailbox_len, &host, &host_len, &personal, &personal_len) == FAILURE) {
		return;
	}

	addr=mail_newaddr();

	if (mailbox) {
		addr->mailbox = cpystr(mailbox);
	}

	if (host) {
		addr->host = cpystr(host);
	}

	if (personal) {
		addr->personal = cpystr(personal);
	}

	addr->next=NIL;
	addr->error=NIL;
	addr->adl=NIL;

	string = _php_rfc822_write_address(addr TSRMLS_CC);
	if (string) {
		RETVAL_STRINGL_CHECK(string, strlen(string), 0);
	} else {
		RETURN_FALSE;
	}