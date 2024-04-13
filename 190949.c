/* Author: CJH */
PHP_FUNCTION(imap_last_error)
{
	ERRORLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (IMAPG(imap_errorstack) == NIL) {
		RETURN_FALSE;
	}

	cur = IMAPG(imap_errorstack);
	while (cur != NIL) {
		if (cur->next == NIL) {
			RETURN_STRING((char*)cur->LTEXT);
		}
		cur = cur->next;
	}