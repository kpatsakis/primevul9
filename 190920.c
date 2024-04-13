
PHP_IMAP_EXPORT void mm_list(MAILSTREAM *stream, DTYPE delimiter, char *mailbox, long attributes)
{
	STRINGLIST *cur=NIL;
	FOBJECTLIST *ocur=NIL;

	if (IMAPG(folderlist_style) == FLIST_OBJECT) {
		/* build up a the new array of objects */
		/* Author: CJH */
		if (IMAPG(imap_folder_objects) == NIL) {
			IMAPG(imap_folder_objects) = mail_newfolderobjectlist();
			IMAPG(imap_folder_objects)->LSIZE=strlen((char*)(IMAPG(imap_folder_objects)->LTEXT = (unsigned char*)cpystr(mailbox)));
			IMAPG(imap_folder_objects)->delimiter = delimiter;
			IMAPG(imap_folder_objects)->attributes = attributes;
			IMAPG(imap_folder_objects)->next = NIL;
			IMAPG(imap_folder_objects_tail) = IMAPG(imap_folder_objects);
		} else {
			ocur=IMAPG(imap_folder_objects_tail);
			ocur->next=mail_newfolderobjectlist();
			ocur=ocur->next;
			ocur->LSIZE = strlen((char*)(ocur->LTEXT = (unsigned char*)cpystr(mailbox)));
			ocur->delimiter = delimiter;
			ocur->attributes = attributes;
			ocur->next = NIL;
			IMAPG(imap_folder_objects_tail) = ocur;
		}

	} else {
		/* build the old IMAPG(imap_folders) variable to allow old imap_listmailbox() to work */
		if (!(attributes & LATT_NOSELECT)) {
			if (IMAPG(imap_folders) == NIL) {
				IMAPG(imap_folders)=mail_newstringlist();
				IMAPG(imap_folders)->LSIZE=strlen((char*)(IMAPG(imap_folders)->LTEXT = (unsigned char*)cpystr(mailbox)));
				IMAPG(imap_folders)->next=NIL;
				IMAPG(imap_folders_tail) = IMAPG(imap_folders);
			} else {
				cur=IMAPG(imap_folders_tail);
				cur->next=mail_newstringlist ();
				cur=cur->next;
				cur->LSIZE = strlen((char*)(cur->LTEXT = (unsigned char*)cpystr(mailbox)));
				cur->next = NIL;
				IMAPG(imap_folders_tail) = cur;
			}
		}
	}