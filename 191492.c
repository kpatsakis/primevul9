int commonio_update (struct commonio_db *db, const void *eptr)
{
	struct commonio_entry *p;
	void *nentry;

	if (!db->isopen || db->readonly) {
		errno = EINVAL;
		return 0;
	}
	nentry = db->ops->dup (eptr);
	if (NULL == nentry) {
		errno = ENOMEM;
		return 0;
	}
	p = find_entry_by_name (db, db->ops->getname (eptr));
	if (NULL != p) {
		if (next_entry_by_name (db, p->next, db->ops->getname (eptr)) != NULL) {
			fprintf (stderr, _("Multiple entries named '%s' in %s. Please fix this with pwck or grpck.\n"), db->ops->getname (eptr), db->filename);
			db->ops->free (nentry);
			return 0;
		}
		db->ops->free (p->eptr);
		p->eptr = nentry;
		p->changed = true;
		db->cursor = p;

		db->changed = true;
		return 1;
	}
	/* not found, new entry */
	p = (struct commonio_entry *) malloc (sizeof *p);
	if (NULL == p) {
		db->ops->free (nentry);
		errno = ENOMEM;
		return 0;
	}

	p->eptr = nentry;
	p->line = NULL;
	p->changed = true;

#if KEEP_NIS_AT_END
	add_one_entry_nis (db, p);
#else				/* !KEEP_NIS_AT_END */
	add_one_entry (db, p);
#endif				/* !KEEP_NIS_AT_END */

	db->changed = true;
	return 1;
}