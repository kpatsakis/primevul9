/* Author: CJH */
PHP_FUNCTION(imap_lsub_full)
{
	zval *streamind, mboxob;
	zend_string *ref, *pat;
	pils *imap_le_struct;
	FOBJECTLIST *cur=NIL;
	char *delim=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS", &streamind, &ref, &pat) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* set flag for new, improved array of objects list */
	IMAPG(folderlist_style) = FLIST_OBJECT;

	IMAPG(imap_sfolder_objects) = IMAPG(imap_sfolder_objects_tail) = NIL;
	mail_lsub(imap_le_struct->imap_stream, ZSTR_VAL(ref), ZSTR_VAL(pat));
	if (IMAPG(imap_sfolder_objects) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	delim = safe_emalloc(2, sizeof(char), 0);
	cur=IMAPG(imap_sfolder_objects);
	while (cur != NIL) {
		object_init(&mboxob);
		add_property_string(&mboxob, "name", (char*)cur->LTEXT);
		add_property_long(&mboxob, "attributes", cur->attributes);
#ifdef IMAP41
		delim[0] = (char)cur->delimiter;
		delim[1] = 0;
		add_property_string(&mboxob, "delimiter", delim);
#else
		add_property_string(&mboxob, "delimiter", cur->delimiter);
#endif
		add_next_index_object(return_value, &mboxob);
		cur=cur->next;
	}
	mail_free_foblist (&IMAPG(imap_sfolder_objects), &IMAPG(imap_sfolder_objects_tail));
	efree(delim);
	IMAPG(folderlist_style) = FLIST_ARRAY; /* reset to default */