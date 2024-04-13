 */
PHP_IMAP_EXPORT void mm_searched(MAILSTREAM *stream, unsigned long number)
{
	MESSAGELIST *cur = NIL;

	if (IMAPG(imap_messages) == NIL) {
		IMAPG(imap_messages) = mail_newmessagelist();
		IMAPG(imap_messages)->msgid = number;
		IMAPG(imap_messages)->next = NIL;
		IMAPG(imap_messages_tail) = IMAPG(imap_messages);
	} else {
		cur = IMAPG(imap_messages_tail);
		cur->next = mail_newmessagelist();
		cur = cur->next;
		cur->msgid = number;
		cur->next = NIL;
		IMAPG(imap_messages_tail) = cur;
	}