int sc_file_add_acl_entry(sc_file_t *file, unsigned int operation,
                          unsigned int method, unsigned long key_ref)
{
	sc_acl_entry_t *p, *_new;

	if (file == NULL || operation >= SC_MAX_AC_OPS) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	switch (method) {
	case SC_AC_NEVER:
		sc_file_clear_acl_entries(file, operation);
		file->acl[operation] = (sc_acl_entry_t *) 1;
		return SC_SUCCESS;
	case SC_AC_NONE:
		sc_file_clear_acl_entries(file, operation);
		file->acl[operation] = (sc_acl_entry_t *) 2;
		return SC_SUCCESS;
	case SC_AC_UNKNOWN:
		sc_file_clear_acl_entries(file, operation);
		file->acl[operation] = (sc_acl_entry_t *) 3;
		return SC_SUCCESS;
	default:
		/* NONE and UNKNOWN get zapped when a new AC is added.
		 * If the ACL is NEVER, additional entries will be
		 * dropped silently. */
		if (file->acl[operation] == (sc_acl_entry_t *) 1)
			return SC_SUCCESS;
		if (file->acl[operation] == (sc_acl_entry_t *) 2
		 || file->acl[operation] == (sc_acl_entry_t *) 3)
			file->acl[operation] = NULL;
	}

	/* If the entry is already present (e.g. due to the mapping)
	 * of the card's AC with OpenSC's), don't add it again. */
	for (p = file->acl[operation]; p != NULL; p = p->next) {
		if ((p->method == method) && (p->key_ref == key_ref))
			return SC_SUCCESS;
	}

	_new = malloc(sizeof(sc_acl_entry_t));
	if (_new == NULL)
		return SC_ERROR_OUT_OF_MEMORY;
	_new->method = method;
	_new->key_ref = key_ref;
	_new->next = NULL;

	p = file->acl[operation];
	if (p == NULL) {
		file->acl[operation] = _new;
		return SC_SUCCESS;
	}
	while (p->next != NULL)
		p = p->next;
	p->next = _new;

	return SC_SUCCESS;
}