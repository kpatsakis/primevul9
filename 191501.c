/*@observer@*/ /*@null@*/const void *commonio_locate (struct commonio_db *db, const char *name)
{
	struct commonio_entry *p;

	if (!db->isopen) {
		errno = EINVAL;
		return NULL;
	}
	p = find_entry_by_name (db, name);
	if (NULL == p) {
		errno = ENOENT;
		return NULL;
	}
	db->cursor = p;
	return p->eptr;
}