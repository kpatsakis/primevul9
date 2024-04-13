
PHP_IMAP_EXPORT void mm_lsub(MAILSTREAM *stream, DTYPE delimiter, char *mailbox, long attributes)
{
	STRINGLIST *cur=NIL;
	FOBJECTLIST *ocur=NIL;

	if (IMAPG(folderlist_style) == FLIST_OBJECT) {
		/* build the array of objects */
		/* Author: CJH */
		if (IMAPG(imap_sfolder_objects) == NIL) {
			IMAPG(imap_sfolder_objects) = mail_newfolderobjectlist();
			IMAPG(imap_sfolder_objects)->LSIZE = strlen((char*)(IMAPG(imap_sfolder_objects)->LTEXT = (unsigned char*)cpystr(mailbox)));
			IMAPG(imap_sfolder_objects)->delimiter = delimiter;
			IMAPG(imap_sfolder_objects)->attributes = attributes;
			IMAPG(imap_sfolder_objects)->next = NIL;
			IMAPG(imap_sfolder_objects_tail) = IMAPG(imap_sfolder_objects);
		} else {
			ocur=IMAPG(imap_sfolder_objects_tail);
			ocur->next=mail_newfolderobjectlist();
			ocur=ocur->next;
			ocur->LSIZE=strlen((char*)(ocur->LTEXT = (unsigned char*)cpystr(mailbox)));
			ocur->delimiter = delimiter;
			ocur->attributes = attributes;
			ocur->next = NIL;
			IMAPG(imap_sfolder_objects_tail) = ocur;
		}
	} else {
		/* build the old simple array for imap_listsubscribed() */
		if (IMAPG(imap_sfolders) == NIL) {
			IMAPG(imap_sfolders)=mail_newstringlist();
			IMAPG(imap_sfolders)->LSIZE=strlen((char*)(IMAPG(imap_sfolders)->LTEXT = (unsigned char*)cpystr(mailbox)));
			IMAPG(imap_sfolders)->next=NIL;
			IMAPG(imap_sfolders_tail) = IMAPG(imap_sfolders);
		} else {
			cur=IMAPG(imap_sfolders_tail);
			cur->next=mail_newstringlist ();
			cur=cur->next;
			cur->LSIZE = strlen((char*)(cur->LTEXT = (unsigned char*)cpystr(mailbox)));
			cur->next = NIL;
			IMAPG(imap_sfolders_tail) = cur;
		}
	}