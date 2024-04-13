const sc_acl_entry_t * sc_file_get_acl_entry(const sc_file_t *file,
						  unsigned int operation)
{
	sc_acl_entry_t *p;
	static const sc_acl_entry_t e_never = {
		SC_AC_NEVER, SC_AC_KEY_REF_NONE, {{0, 0, 0, {0}}}, NULL
	};
	static const sc_acl_entry_t e_none = {
		SC_AC_NONE, SC_AC_KEY_REF_NONE, {{0, 0, 0, {0}}}, NULL
	};
	static const sc_acl_entry_t e_unknown = {
		SC_AC_UNKNOWN, SC_AC_KEY_REF_NONE, {{0, 0, 0, {0}}}, NULL
	};

	if (file == NULL || operation >= SC_MAX_AC_OPS) {
		return NULL;
	}

	p = file->acl[operation];
	if (p == (sc_acl_entry_t *) 1)
		return &e_never;
	if (p == (sc_acl_entry_t *) 2)
		return &e_none;
	if (p == (sc_acl_entry_t *) 3)
		return &e_unknown;

	return file->acl[operation];
}