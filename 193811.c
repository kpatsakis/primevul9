static unsigned int acl_to_byte_44(const struct sc_acl_entry *e, u8* p_bNumber)
{
	/* Handle special fixed values */
	if (e == (sc_acl_entry_t *) 1)           /* SC_AC_NEVER */
		return SC_AC_NEVER;
	else if ((e == (sc_acl_entry_t *) 2) ||  /* SC_AC_NONE */
	         (e == (sc_acl_entry_t *) 3) ||  /* SC_AC_UNKNOWN */
	         (e == (sc_acl_entry_t *) 0))
		return SC_AC_NONE;

	/* Handle standard values */
	*p_bNumber = e->key_ref;
	return(e->method);
}