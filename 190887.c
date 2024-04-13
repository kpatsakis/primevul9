   Get status info from a mailbox */
PHP_FUNCTION(imap_status)
{
	zval *streamind;
	zend_string *mbx;
	zend_long flags;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSl", &streamind, &mbx, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	object_init(return_value);

	if (mail_status(imap_le_struct->imap_stream, ZSTR_VAL(mbx), flags)) {
		add_property_long(return_value, "flags", IMAPG(status_flags));
		if (IMAPG(status_flags) & SA_MESSAGES) {
			add_property_long(return_value, "messages", IMAPG(status_messages));
		}
		if (IMAPG(status_flags) & SA_RECENT) {
			add_property_long(return_value, "recent", IMAPG(status_recent));
		}
		if (IMAPG(status_flags) & SA_UNSEEN) {
			add_property_long(return_value, "unseen", IMAPG(status_unseen));
		}
		if (IMAPG(status_flags) & SA_UIDNEXT) {
			add_property_long(return_value, "uidnext", IMAPG(status_uidnext));
		}
		if (IMAPG(status_flags) & SA_UIDVALIDITY) {
			add_property_long(return_value, "uidvalidity", IMAPG(status_uidvalidity));
		}
	} else {
		RETURN_FALSE;
	}