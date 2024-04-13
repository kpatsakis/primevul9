   Read the list of mailboxes */
PHP_FUNCTION(imap_list)
{
	zval *streamind;
	zend_string *ref, *pat;
	pils *imap_le_struct;
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS", &streamind, &ref, &pat) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* set flag for normal, old mailbox list */
	IMAPG(folderlist_style) = FLIST_ARRAY;

	IMAPG(imap_folders) = IMAPG(imap_folders_tail) = NIL;
	mail_list(imap_le_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
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