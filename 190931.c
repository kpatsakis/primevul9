 */
void mail_getacl(MAILSTREAM *stream, char *mailbox, ACLLIST *alist)
{

	/* walk through the ACLLIST */
	for(; alist; alist = alist->next) {
		add_assoc_stringl(IMAPG(imap_acl_list), alist->identifier, alist->rights, strlen(alist->rights));
	}