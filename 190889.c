   Returns info about the current mailbox */
PHP_FUNCTION(imap_mailboxmsginfo)
{
	zval *streamind;
	pils *imap_le_struct;
	char date[100];
	unsigned long msgno;
	zend_ulong unreadmsg = 0, deletedmsg = 0, msize = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* Initialize return object */
	object_init(return_value);

	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream, msgno);
		mail_fetchstructure (imap_le_struct->imap_stream, msgno, NIL);

		if (!cache->seen || cache->recent) {
			unreadmsg++;
		}

		if (cache->deleted) {
			deletedmsg++;
		}
		msize = msize + cache->rfc822_size;
	}
	add_property_long(return_value, "Unread", unreadmsg);
	add_property_long(return_value, "Deleted", deletedmsg);
	add_property_long(return_value, "Nmsgs", imap_le_struct->imap_stream->nmsgs);
	add_property_long(return_value, "Size", msize);
	rfc822_date(date);
	add_property_string(return_value, "Date", date);
	add_property_string(return_value, "Driver", imap_le_struct->imap_stream->dtb->name);
	add_property_string(return_value, "Mailbox", imap_le_struct->imap_stream->mailbox);
	add_property_long(return_value, "Recent", imap_le_struct->imap_stream->recent);