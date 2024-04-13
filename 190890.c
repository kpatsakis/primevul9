   Read list of mailboxes containing a certain string */
PHP_FUNCTION(imap_listscan)
{
	zval *streamind;
	zend_string *ref, *pat, *content;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSSS", &streamind, &ref, &pat, &content) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	IMAPG(imap_folders) = NIL;
	mail_scan(imap_le_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat), ZSTR_VAL(content));
	if (IMAPG(imap_folders) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	cur=IMAPG(imap_folders);
	while (cur != NIL) {
		add_next_index_string(return_value, (char*)cur->LTEXT);
		cur=cur->next;
	}
	mail_free_stringlist (&IMAPG(imap_folders));
	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;