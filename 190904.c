/* Author: CJH */
PHP_FUNCTION(imap_errors)
{
	ERRORLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (IMAPG(imap_errorstack) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	cur = IMAPG(imap_errorstack);
	while (cur != NIL) {
		add_next_index_string(return_value, (char*)cur->LTEXT);
		cur = cur->next;
	}
	mail_free_errorlist(&IMAPG(imap_errorstack));
	IMAPG(imap_errorstack) = NIL;