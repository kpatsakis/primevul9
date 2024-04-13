int commonio_remove (struct commonio_db *db, const char *name)
{
	struct commonio_entry *p;

	if (!db->isopen || db->readonly) {
		errno = EINVAL;
		return 0;
	}
	p = find_entry_by_name (db, name);
	if (NULL == p) {
		errno = ENOENT;
		return 0;
	}
	if (next_entry_by_name (db, p->next, name) != NULL) {
		fprintf (stderr, _("Multiple entries named '%s' in %s. Please fix this with pwck or grpck.\n"), name, db->filename);
		return 0;
	}

	commonio_del_entry (db, p);

	if (NULL != p->line) {
		free (p->line);
	}

	if (NULL != p->eptr) {
		db->ops->free (p->eptr);
	}

	return 1;
}