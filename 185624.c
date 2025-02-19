 */
PHP_MINIT_FUNCTION(imap)
{
	unsigned long sa_all =	SA_MESSAGES | SA_RECENT | SA_UNSEEN | SA_UIDNEXT | SA_UIDVALIDITY;

	REGISTER_INI_ENTRIES();

#ifndef PHP_WIN32
	mail_link(&unixdriver);		/* link in the unix driver */
	mail_link(&mhdriver);		/* link in the mh driver */
	/* mail_link(&mxdriver); */	/* According to c-client docs (internal.txt) this shouldn't be used. */
	mail_link(&mmdfdriver);		/* link in the mmdf driver */
	mail_link(&newsdriver);		/* link in the news driver */
	mail_link(&philedriver);	/* link in the phile driver */
#endif
	mail_link(&imapdriver);		/* link in the imap driver */
	mail_link(&nntpdriver);		/* link in the nntp driver */
	mail_link(&pop3driver);		/* link in the pop3 driver */
	mail_link(&mbxdriver);		/* link in the mbx driver */
	mail_link(&tenexdriver);	/* link in the tenex driver */
	mail_link(&mtxdriver);		/* link in the mtx driver */
	mail_link(&dummydriver);	/* link in the dummy driver */

#ifndef PHP_WIN32
	auth_link(&auth_log);		/* link in the log authenticator */
	auth_link(&auth_md5);		/* link in the cram-md5 authenticator */
#if HAVE_IMAP_KRB && defined(HAVE_IMAP_AUTH_GSS)
	auth_link(&auth_gss);		/* link in the gss authenticator */
#endif
	auth_link(&auth_pla);		/* link in the plain authenticator */
#endif

#ifdef HAVE_IMAP_SSL
	ssl_onceonlyinit ();
#endif

	/* lets allow NIL */
	REGISTER_LONG_CONSTANT("NIL", NIL, CONST_PERSISTENT | CONST_CS);

	/* plug in our gets */
	mail_parameters(NIL, SET_GETS, (void *) NIL);

	/* set default timeout values */
	mail_parameters(NIL, SET_OPENTIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_READTIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_WRITETIMEOUT, (void *) FG(default_socket_timeout));
	mail_parameters(NIL, SET_CLOSETIMEOUT, (void *) FG(default_socket_timeout));

	/* timeout constants */
	REGISTER_LONG_CONSTANT("IMAP_OPENTIMEOUT", 1, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_READTIMEOUT", 2, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_WRITETIMEOUT", 3, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_CLOSETIMEOUT", 4, CONST_PERSISTENT | CONST_CS);

	/* Open Options */

	REGISTER_LONG_CONSTANT("OP_DEBUG", OP_DEBUG, CONST_PERSISTENT | CONST_CS);
	/* debug protocol negotiations */
	REGISTER_LONG_CONSTANT("OP_READONLY", OP_READONLY, CONST_PERSISTENT | CONST_CS);
	/* read-only open */
	REGISTER_LONG_CONSTANT("OP_ANONYMOUS", OP_ANONYMOUS, CONST_PERSISTENT | CONST_CS);
	/* anonymous open of newsgroup */
	REGISTER_LONG_CONSTANT("OP_SHORTCACHE", OP_SHORTCACHE, CONST_PERSISTENT | CONST_CS);
	/* short (elt-only) caching */
	REGISTER_LONG_CONSTANT("OP_SILENT", OP_SILENT, CONST_PERSISTENT | CONST_CS);
	/* don't pass up events (internal use) */
	REGISTER_LONG_CONSTANT("OP_PROTOTYPE", OP_PROTOTYPE, CONST_PERSISTENT | CONST_CS);
	/* return driver prototype */
	REGISTER_LONG_CONSTANT("OP_HALFOPEN", OP_HALFOPEN, CONST_PERSISTENT | CONST_CS);
	/* half-open (IMAP connect but no select) */
	REGISTER_LONG_CONSTANT("OP_EXPUNGE", OP_EXPUNGE, CONST_PERSISTENT | CONST_CS);
	/* silently expunge recycle stream */
	REGISTER_LONG_CONSTANT("OP_SECURE", OP_SECURE, CONST_PERSISTENT | CONST_CS);
	/* don't do non-secure authentication */

	/*
	PHP re-assigns CL_EXPUNGE a custom value that can be used as part of the imap_open() bitfield
	because it seems like a good idea to be able to indicate that the mailbox should be
	automatically expunged during imap_open in case the script get interrupted and it doesn't get
	to the imap_close() where this option is normally placed.  If the c-client library adds other
	options and the value for this one conflicts, simply make PHP_EXPUNGE higher at the top of
	this file
	*/
	REGISTER_LONG_CONSTANT("CL_EXPUNGE", PHP_EXPUNGE, CONST_PERSISTENT | CONST_CS);
	/* expunge silently */

	/* Fetch options */

	REGISTER_LONG_CONSTANT("FT_UID", FT_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID */
	REGISTER_LONG_CONSTANT("FT_PEEK", FT_PEEK, CONST_PERSISTENT | CONST_CS);
	/* peek at data */
	REGISTER_LONG_CONSTANT("FT_NOT", FT_NOT, CONST_PERSISTENT | CONST_CS);
	/* NOT flag for header lines fetch */
	REGISTER_LONG_CONSTANT("FT_INTERNAL", FT_INTERNAL, CONST_PERSISTENT | CONST_CS);
	/* text can be internal strings */
	REGISTER_LONG_CONSTANT("FT_PREFETCHTEXT", FT_PREFETCHTEXT, CONST_PERSISTENT | CONST_CS);
	/* IMAP prefetch text when fetching header */

	/* Flagging options */

	REGISTER_LONG_CONSTANT("ST_UID", ST_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID sequence */
	REGISTER_LONG_CONSTANT("ST_SILENT", ST_SILENT, CONST_PERSISTENT | CONST_CS);
	/* don't return results */
	REGISTER_LONG_CONSTANT("ST_SET", ST_SET, CONST_PERSISTENT | CONST_CS);
	/* set vs. clear */

	/* Copy options */

	REGISTER_LONG_CONSTANT("CP_UID", CP_UID, CONST_PERSISTENT | CONST_CS);
	/* argument is a UID sequence */
	REGISTER_LONG_CONSTANT("CP_MOVE", CP_MOVE, CONST_PERSISTENT | CONST_CS);
	/* delete from source after copying */

	/* Search/sort options */

	REGISTER_LONG_CONSTANT("SE_UID", SE_UID, CONST_PERSISTENT | CONST_CS);
	/* return UID */
	REGISTER_LONG_CONSTANT("SE_FREE", SE_FREE, CONST_PERSISTENT | CONST_CS);
	/* free search program after finished */
	REGISTER_LONG_CONSTANT("SE_NOPREFETCH", SE_NOPREFETCH, CONST_PERSISTENT | CONST_CS);
	/* no search prefetching */
	REGISTER_LONG_CONSTANT("SO_FREE", SO_FREE, CONST_PERSISTENT | CONST_CS);
	/* free sort program after finished */
	REGISTER_LONG_CONSTANT("SO_NOSERVER", SO_NOSERVER, CONST_PERSISTENT | CONST_CS);
	/* don't do server-based sort */

	/* Status options */

	REGISTER_LONG_CONSTANT("SA_MESSAGES", SA_MESSAGES , CONST_PERSISTENT | CONST_CS);
	/* number of messages */
	REGISTER_LONG_CONSTANT("SA_RECENT", SA_RECENT, CONST_PERSISTENT | CONST_CS);
	/* number of recent messages */
	REGISTER_LONG_CONSTANT("SA_UNSEEN", SA_UNSEEN , CONST_PERSISTENT | CONST_CS);
	/* number of unseen messages */
	REGISTER_LONG_CONSTANT("SA_UIDNEXT", SA_UIDNEXT, CONST_PERSISTENT | CONST_CS);
	/* next UID to be assigned */
	REGISTER_LONG_CONSTANT("SA_UIDVALIDITY", SA_UIDVALIDITY , CONST_PERSISTENT | CONST_CS);
	/* UID validity value */
	REGISTER_LONG_CONSTANT("SA_ALL", sa_all, CONST_PERSISTENT | CONST_CS);
	/* get all status information */

	/* Bits for mm_list() and mm_lsub() */

	REGISTER_LONG_CONSTANT("LATT_NOINFERIORS", LATT_NOINFERIORS , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LATT_NOSELECT", LATT_NOSELECT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LATT_MARKED", LATT_MARKED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("LATT_UNMARKED", LATT_UNMARKED , CONST_PERSISTENT | CONST_CS);

#ifdef LATT_REFERRAL
	REGISTER_LONG_CONSTANT("LATT_REFERRAL", LATT_REFERRAL, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LATT_HASCHILDREN
	REGISTER_LONG_CONSTANT("LATT_HASCHILDREN", LATT_HASCHILDREN, CONST_PERSISTENT | CONST_CS);
#endif

#ifdef LATT_HASNOCHILDREN
	REGISTER_LONG_CONSTANT("LATT_HASNOCHILDREN", LATT_HASNOCHILDREN, CONST_PERSISTENT | CONST_CS);
#endif

	/* Sort functions */

	REGISTER_LONG_CONSTANT("SORTDATE", SORTDATE , CONST_PERSISTENT | CONST_CS);
	/* date */
	REGISTER_LONG_CONSTANT("SORTARRIVAL", SORTARRIVAL , CONST_PERSISTENT | CONST_CS);
	/* arrival date */
	REGISTER_LONG_CONSTANT("SORTFROM", SORTFROM , CONST_PERSISTENT | CONST_CS);
	/* from */
	REGISTER_LONG_CONSTANT("SORTSUBJECT", SORTSUBJECT , CONST_PERSISTENT | CONST_CS);
	/* subject */
	REGISTER_LONG_CONSTANT("SORTTO", SORTTO , CONST_PERSISTENT | CONST_CS);
	/* to */
	REGISTER_LONG_CONSTANT("SORTCC", SORTCC , CONST_PERSISTENT | CONST_CS);
	/* cc */
	REGISTER_LONG_CONSTANT("SORTSIZE", SORTSIZE , CONST_PERSISTENT | CONST_CS);
	/* size */

	REGISTER_LONG_CONSTANT("TYPETEXT", TYPETEXT , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEMULTIPART", TYPEMULTIPART , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEMESSAGE", TYPEMESSAGE , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEAPPLICATION", TYPEAPPLICATION , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEAUDIO", TYPEAUDIO , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEIMAGE", TYPEIMAGE , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEVIDEO", TYPEVIDEO , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEMODEL", TYPEMODEL , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("TYPEOTHER", TYPEOTHER , CONST_PERSISTENT | CONST_CS);
	/*
	TYPETEXT                unformatted text
	TYPEMULTIPART           multiple part
	TYPEMESSAGE             encapsulated message
	TYPEAPPLICATION         application data
	TYPEAUDIO               audio
	TYPEIMAGE               static image (GIF, JPEG, etc.)
	TYPEVIDEO               video
	TYPEMODEL               model
	TYPEOTHER               unknown
	*/

	REGISTER_LONG_CONSTANT("ENC7BIT", ENC7BIT , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENC8BIT", ENC8BIT , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENCBINARY", ENCBINARY , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENCBASE64", ENCBASE64, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENCQUOTEDPRINTABLE", ENCQUOTEDPRINTABLE , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("ENCOTHER", ENCOTHER , CONST_PERSISTENT | CONST_CS);
	/*
	ENC7BIT                 7 bit SMTP semantic data
	ENC8BIT                 8 bit SMTP semantic data
	ENCBINARY               8 bit binary data
	ENCBASE64               base-64 encoded data
	ENCQUOTEDPRINTABLE      human-readable 8-as-7 bit data
	ENCOTHER                unknown
	*/

	REGISTER_LONG_CONSTANT("IMAP_GC_ELT", GC_ELT , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_GC_ENV", GC_ENV , CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("IMAP_GC_TEXTS", GC_TEXTS , CONST_PERSISTENT | CONST_CS);
	/*
	GC_ELT                 message cache elements
	GC_ENV                 ENVELOPEs and BODYs
	GC_TEXTS               texts
	*/

	if (!IMAPG(enable_rsh)) {
		/* disable SSH and RSH, see https://bugs.php.net/bug.php?id=77153 */
		mail_parameters (NIL, SET_RSHTIMEOUT, 0);
		mail_parameters (NIL, SET_SSHTIMEOUT, 0);
	}

	le_imap = zend_register_list_destructors_ex(mail_close_it, NULL, "imap", module_number);
	return SUCCESS;