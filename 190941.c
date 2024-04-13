
PHP_IMAP_EXPORT void mm_status(MAILSTREAM *stream, char *mailbox, MAILSTATUS *status)
{

	IMAPG(status_flags)=status->flags;
	if (IMAPG(status_flags) & SA_MESSAGES) {
		IMAPG(status_messages)=status->messages;
	}
	if (IMAPG(status_flags) & SA_RECENT) {
		IMAPG(status_recent)=status->recent;
	}
	if (IMAPG(status_flags) & SA_UNSEEN) {
		IMAPG(status_unseen)=status->unseen;
	}
	if (IMAPG(status_flags) & SA_UIDNEXT) {
		IMAPG(status_uidnext)=status->uidnext;
	}
	if (IMAPG(status_flags) & SA_UIDVALIDITY) {
		IMAPG(status_uidvalidity)=status->uidvalidity;
	}