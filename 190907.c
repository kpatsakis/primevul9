/* Author: CJH */
PHP_FUNCTION(imap_alerts)
{
	STRINGLIST *cur=NIL;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (IMAPG(imap_alertstack) == NIL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	cur = IMAPG(imap_alertstack);
	while (cur != NIL) {
		add_next_index_string(return_value, (char*)cur->LTEXT);
		cur = cur->next;
	}
	mail_free_stringlist(&IMAPG(imap_alertstack));
	IMAPG(imap_alertstack) = NIL;