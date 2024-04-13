   Returns headers for all messages in a mailbox */
PHP_FUNCTION(imap_headers)
{
	zval *streamind;
	pils *imap_le_struct;
	unsigned long i;
	char *t;
	unsigned int msgno;
	char tmp[MAILTMPLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* Initialize return array */
	array_init(return_value);

	for (msgno = 1; msgno <= imap_le_struct->imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_le_struct->imap_stream, msgno);
		mail_fetchstructure(imap_le_struct->imap_stream, msgno, NIL);
		tmp[0] = cache->recent ? (cache->seen ? 'R': 'N') : ' ';
		tmp[1] = (cache->recent | cache->seen) ? ' ' : 'U';
		tmp[2] = cache->flagged ? 'F' : ' ';
		tmp[3] = cache->answered ? 'A' : ' ';
		tmp[4] = cache->deleted ? 'D' : ' ';
		tmp[5] = cache->draft ? 'X' : ' ';
		snprintf(tmp + 6, sizeof(tmp) - 6, "%4ld) ", cache->msgno);
		mail_date(tmp+11, cache);
		tmp[22] = ' ';
		tmp[23] = '\0';
		mail_fetchfrom(tmp+23, imap_le_struct->imap_stream, msgno, (long)20);
		strcat(tmp, " ");
		if ((i = cache->user_flags)) {
			strcat(tmp, "{");
			while (i) {
				strlcat(tmp, imap_le_struct->imap_stream->user_flags[find_rightmost_bit (&i)], sizeof(tmp));
				if (i) strlcat(tmp, " ", sizeof(tmp));
			}
			strlcat(tmp, "} ", sizeof(tmp));
		}
		mail_fetchsubject(t = tmp + strlen(tmp), imap_le_struct->imap_stream, msgno, (long)25);
		snprintf(t += strlen(t), sizeof(tmp) - strlen(tmp), " (%ld chars)", cache->rfc822_size);
		add_next_index_string(return_value, tmp);
	}