int commonio_append (struct commonio_db *db, const void *eptr)
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
	/* new entry */
	p = (struct commonio_entry *) malloc (sizeof *p);
	if (NULL == p) {
		db->ops->free (nentry);
		errno = ENOMEM;
		return 0;
	}

	p->eptr = nentry;
	p->line = NULL;
	p->changed = true;
	add_one_entry (db, p);

	db->changed = true;
	return 1;
}