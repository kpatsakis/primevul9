   Return threaded by REFERENCES tree */
PHP_FUNCTION(imap_thread)
{
	zval *streamind;
	pils *imap_le_struct;
	zend_long flags = SE_FREE;
	char criteria[] = "ALL";
	THREADNODE *top;
	int argc = ZEND_NUM_ARGS();
	SEARCHPGM *pgm = NIL;

	if (zend_parse_parameters(argc, "r|l", &streamind, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	pgm = mail_criteria(criteria);
	top = mail_thread(imap_le_struct->imap_stream, "REFERENCES", NIL, pgm, flags);
	if (pgm && !(flags & SE_FREE)) {
		mail_free_searchpgm(&pgm);
	}

	if(top == NIL) {
		php_error_docref(NULL, E_WARNING, "Function returned an empty tree");
		RETURN_FALSE;
	}

	/* Populate our return value data structure here. */
	if(build_thread_tree(top, &return_value) == FAILURE) {
		mail_free_threadnode(&top);
		RETURN_FALSE;
	}
	mail_free_threadnode(&top);