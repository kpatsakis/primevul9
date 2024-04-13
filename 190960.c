/* Author: CJH */
PHP_IMAP_EXPORT void mm_notify(MAILSTREAM *stream, char *str, long errflg)
{
	STRINGLIST *cur = NIL;

	if (strncmp(str, "[ALERT] ", 8) == 0) {
		if (IMAPG(imap_alertstack) == NIL) {
			IMAPG(imap_alertstack) = mail_newstringlist();
			IMAPG(imap_alertstack)->LSIZE = strlen((char*)(IMAPG(imap_alertstack)->LTEXT = (unsigned char*)cpystr(str)));
			IMAPG(imap_alertstack)->next = NIL;
		} else {
			cur = IMAPG(imap_alertstack);
			while (cur->next != NIL) {
				cur = cur->next;
			}
			cur->next = mail_newstringlist ();
			cur = cur->next;
			cur->LSIZE = strlen((char*)(cur->LTEXT = (unsigned char*)cpystr(str)));
			cur->next = NIL;
		}
	}