   Sort an array of message headers, optionally including only messages that meet specified criteria. */
PHP_FUNCTION(imap_sort)
{
	zval *streamind;
	zend_string *criteria = NULL, *charset = NULL;
	zend_long pgm, rev, flags = 0;
	pils *imap_le_struct;
	unsigned long *slst, *sl;
	char *search_criteria;
	SORTPGM *mypgm=NIL;
	SEARCHPGM *spg=NIL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rll|lSS", &streamind, &pgm, &rev, &flags, &criteria, &charset) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (pgm > SORTSIZE) {
		php_error_docref(NULL, E_WARNING, "Unrecognized sort criteria");
		RETURN_FALSE;
	}
	if (argc >= 4) {
		if (flags < 0) {
			php_error_docref(NULL, E_WARNING, "Search options parameter has to be greater than or equal to 0");
			RETURN_FALSE;
		}
	}
	if (argc >= 5) {
		search_criteria = estrndup(ZSTR_VAL(criteria), ZSTR_LEN(criteria));
		spg = mail_criteria(search_criteria);
		efree(search_criteria);
	} else {
		spg = mail_newsearchpgm();
	}

	mypgm = mail_newsortpgm();
	mypgm->reverse = rev;
	mypgm->function = (short) pgm;
	mypgm->next = NIL;

	slst = mail_sort(imap_le_struct->imap_stream, (argc == 6 ? ZSTR_VAL(charset) : NIL), spg, mypgm, (argc >= 4 ? flags : NIL));

	if (spg && !(flags & SE_FREE)) {
		mail_free_searchpgm(&spg);
	}

	array_init(return_value);
	if (slst != NIL && slst != 0) {
		for (sl = slst; *sl; sl++) {
			add_next_index_long(return_value, *sl);
		}
		fs_give ((void **) &slst);
	}