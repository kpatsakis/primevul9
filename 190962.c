   Get mailbox properties */
PHP_FUNCTION(imap_check)
{
	zval *streamind;
	pils *imap_le_struct;
	char date[100];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_ping (imap_le_struct->imap_stream) == NIL) {
		RETURN_FALSE;
	}

	if (imap_le_struct->imap_stream && imap_le_struct->imap_stream->mailbox) {
		rfc822_date(date);
		object_init(return_value);
		add_property_string(return_value, "Date", date);
		add_property_string(return_value, "Driver", imap_le_struct->imap_stream->dtb->name);
		add_property_string(return_value, "Mailbox", imap_le_struct->imap_stream->mailbox);
		add_property_long(return_value, "Nmsgs", imap_le_struct->imap_stream->nmsgs);
		add_property_long(return_value, "Recent", imap_le_struct->imap_stream->recent);
	} else {
		RETURN_FALSE;
	}