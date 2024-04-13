void sc_file_clear_acl_entries(sc_file_t *file, unsigned int operation)
{
	sc_acl_entry_t *e;

	if (file == NULL || operation >= SC_MAX_AC_OPS) {
		return;
	}

	e = file->acl[operation];
	if (e == (sc_acl_entry_t *) 1 ||
	    e == (sc_acl_entry_t *) 2 ||
	    e == (sc_acl_entry_t *) 3) {
		file->acl[operation] = NULL;
		return;
	}

	while (e != NULL) {
		sc_acl_entry_t *tmp = e->next;
		free(e);
		e = tmp;
	}
	file->acl[operation] = NULL;
}