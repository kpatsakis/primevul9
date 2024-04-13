static unsigned short muscle_parse_singleAcl(const sc_acl_entry_t* acl)
{
	unsigned short acl_entry = 0;
	while(acl) {
		int key = acl->key_ref;
		int method = acl->method;
		switch(method) {
		case SC_AC_NEVER:
			return 0xFFFF;
		/* Ignore... other items overwrite these */
		case SC_AC_NONE:
		case SC_AC_UNKNOWN:
			break;
		case SC_AC_CHV:
			acl_entry |= (1 << key); /* Assuming key 0 == SO */
			break;
		case SC_AC_AUT:
		case SC_AC_TERM:
		case SC_AC_PRO:
		default:
			/* Ignored */
			break;
		}
		acl = acl->next;
	}
	return acl_entry;
}