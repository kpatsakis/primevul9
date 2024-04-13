 */
static void mail_close_it(zend_resource *rsrc)
{
	pils *imap_le_struct = (pils *)rsrc->ptr;

	/* Do not try to close prototype streams */
	if (!(imap_le_struct->flags & OP_PROTOTYPE)) {
		mail_close_full(imap_le_struct->imap_stream, imap_le_struct->flags);
	}

	if (IMAPG(imap_user)) {
		efree(IMAPG(imap_user));
		IMAPG(imap_user) = 0;
	}
	if (IMAPG(imap_password)) {
		efree(IMAPG(imap_password));
		IMAPG(imap_password) = 0;
	}

	efree(imap_le_struct);