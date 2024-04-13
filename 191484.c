static /*@dependent@*/ /*@null@*/struct commonio_entry *next_entry_by_name (
	struct commonio_db *db,
	/*@null@*/struct commonio_entry *pos,
	const char *name)
{
	struct commonio_entry *p;
	void *ep;

	if (NULL == pos) {
		return NULL;
	}

	for (p = pos; NULL != p; p = p->next) {
		ep = p->eptr;
		if (   (NULL != ep)
		    && (strcmp (db->ops->getname (ep), name) == 0)) {
			break;
		}
	}
	return p;
}