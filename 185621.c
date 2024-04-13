/* Author: CJH */
PHP_FUNCTION(imap_list_full)
{
	zval *streamind, *mboxob;
	char *ref, *pat;
	int ref_len, pat_len;
	pils *imap_le_struct;
	FOBJECTLIST *cur=NIL;
	char *delim=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &streamind, &ref, &ref_len, &pat, &pat_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, &streamind, -1, "imap", le_imap);

	/* set flag for new, improved array of objects mailbox list */
	IMAPG(folderlist_style) = FLIST_OBJECT;

	IMAPG(imap_folder_objects) = IMAPG(imap_folder_objects_tail) = NIL;
	mail_list(imap_le_struct->imap_stream, ref, pat);
	if (IMAPG(imap_folder_objects) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	delim = emalloc(2);
	cur=IMAPG(imap_folder_objects);
	while (cur != NIL) {
		MAKE_STD_ZVAL(mboxob);
		object_init(mboxob);
		add_property_string(mboxob, "name", cur->LTEXT, 1);
		add_property_long(mboxob, "attributes", cur->attributes);
#ifdef IMAP41
		delim[0] = (char)cur->delimiter;
		delim[1] = 0;
		add_property_string(mboxob, "delimiter", delim, 1);
#else
		add_property_string(mboxob, "delimiter", cur->delimiter, 1);
#endif
		add_next_index_object(return_value, mboxob TSRMLS_CC);
		cur=cur->next;
	}
	mail_free_foblist(&IMAPG(imap_folder_objects), &IMAPG(imap_folder_objects_tail));
	efree(delim);
	IMAPG(folderlist_style) = FLIST_ARRAY;		/* reset to default */