
PHP_IMAP_EXPORT void mm_log(char *str, long errflg)
{
	ERRORLIST *cur = NIL;

	/* Author: CJH */
	if (errflg != NIL) { /* CJH: maybe put these into a more comprehensive log for debugging purposes? */
		if (IMAPG(imap_errorstack) == NIL) {
			IMAPG(imap_errorstack) = mail_newerrorlist();
			IMAPG(imap_errorstack)->LSIZE = strlen((char*)(IMAPG(imap_errorstack)->LTEXT = (unsigned char*)cpystr(str)));
			IMAPG(imap_errorstack)->errflg = errflg;
			IMAPG(imap_errorstack)->next = NIL;
		} else {
			cur = IMAPG(imap_errorstack);
			while (cur->next != NIL) {
				cur = cur->next;
			}
			cur->next = mail_newerrorlist();
			cur = cur->next;
			cur->LSIZE = strlen((char*)(cur->LTEXT = (unsigned char*)cpystr(str)));
			cur->errflg = errflg;
			cur->next = NIL;
		}
	}