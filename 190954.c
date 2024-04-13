   Return a list of messages matching the given criteria */
PHP_FUNCTION(imap_search)
{
	zval *streamind;
	zend_string *criteria, *charset = NULL;
	zend_long flags = SE_FREE;
	pils *imap_le_struct;
	char *search_criteria;
	MESSAGELIST *cur;
	int argc = ZEND_NUM_ARGS();
	SEARCHPGM *pgm = NIL;

	if (zend_parse_parameters(argc, "rS|lS", &streamind, &criteria, &flags, &charset) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	search_criteria = estrndup(ZSTR_VAL(criteria), ZSTR_LEN(criteria));

	IMAPG(imap_messages) = IMAPG(imap_messages_tail) = NIL;
	pgm = mail_criteria(search_criteria);

	mail_search_full(imap_le_struct->imap_stream, (argc == 4 ? ZSTR_VAL(charset) : NIL), pgm, flags);

	if (pgm && !(flags & SE_FREE)) {
		mail_free_searchpgm(&pgm);
	}

	if (IMAPG(imap_messages) == NIL) {
		efree(search_criteria);
		RETURN_FALSE;
	}

	array_init(return_value);

	cur = IMAPG(imap_messages);
	while (cur != NIL) {
		add_next_index_long(return_value, cur->msgid);
		cur = cur->next;
	}
	mail_free_messagelist(&IMAPG(imap_messages), &IMAPG(imap_messages_tail));
	efree(search_criteria);