   Read the headers of the message */
PHP_FUNCTION(imap_headerinfo)
{
	zval *streamind;
	zend_string *defaulthost = NULL;
	int argc = ZEND_NUM_ARGS();
	zend_long msgno, fromlength, subjectlength;
	pils *imap_le_struct;
	MESSAGECACHE *cache;
	ENVELOPE *en;
	char dummy[2000], fulladdress[MAILTMPLEN + 1];

	if (zend_parse_parameters(argc, "rl|llS", &streamind, &msgno, &fromlength, &subjectlength, &defaulthost) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (argc >= 3) {
		if (fromlength < 0 || fromlength > MAILTMPLEN) {
			php_error_docref(NULL, E_WARNING, "From length has to be between 0 and %d", MAILTMPLEN);
			RETURN_FALSE;
		}
	} else {
		fromlength = 0x00;
	}
	if (argc >= 4) {
		if (subjectlength < 0 || subjectlength > MAILTMPLEN) {
			php_error_docref(NULL, E_WARNING, "Subject length has to be between 0 and %d", MAILTMPLEN);
			RETURN_FALSE;
		}
	} else {
		subjectlength = 0x00;
	}

	PHP_IMAP_CHECK_MSGNO(msgno);

	if (mail_fetchstructure(imap_le_struct->imap_stream, msgno, NIL)) {
		cache = mail_elt(imap_le_struct->imap_stream, msgno);
	} else {
		RETURN_FALSE;
	}

	en = mail_fetchenvelope(imap_le_struct->imap_stream, msgno);

	/* call a function to parse all the text, so that we can use the
	   same function to parse text from other sources */
	_php_make_header_object(return_value, en);

	/* now run through properties that are only going to be returned
	   from a server, not text headers */
	add_property_string(return_value, "Recent", cache->recent ? (cache->seen ? "R": "N") : " ");
	add_property_string(return_value, "Unseen", (cache->recent | cache->seen) ? " " : "U");
	add_property_string(return_value, "Flagged", cache->flagged ? "F" : " ");
	add_property_string(return_value, "Answered", cache->answered ? "A" : " ");
	add_property_string(return_value, "Deleted", cache->deleted ? "D" : " ");
	add_property_string(return_value, "Draft", cache->draft ? "X" : " ");

	snprintf(dummy, sizeof(dummy), "%4ld", cache->msgno);
	add_property_string(return_value, "Msgno", dummy);

	mail_date(dummy, cache);
	add_property_string(return_value, "MailDate", dummy);

	snprintf(dummy, sizeof(dummy), "%ld", cache->rfc822_size);
	add_property_string(return_value, "Size", dummy);

	add_property_long(return_value, "udate", mail_longdate(cache));

	if (en->from && fromlength) {
		fulladdress[0] = 0x00;
		mail_fetchfrom(fulladdress, imap_le_struct->imap_stream, msgno, fromlength);
		add_property_string(return_value, "fetchfrom", fulladdress);
	}
	if (en->subject && subjectlength) {
		fulladdress[0] = 0x00;
		mail_fetchsubject(fulladdress, imap_le_struct->imap_stream, msgno, subjectlength);
		add_property_string(return_value, "fetchsubject", fulladdress);
	}