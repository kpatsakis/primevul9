static int write_all (const struct commonio_db *db)
	/*@requires notnull db->fp@*/
{
	const struct commonio_entry *p;
	void *eptr;

	for (p = db->head; NULL != p; p = p->next) {
		if (p->changed) {
			eptr = p->eptr;
			assert (NULL != eptr);
			if (db->ops->put (eptr, db->fp) != 0) {
				return -1;
			}
		} else if (NULL != p->line) {
			if (db->ops->fputs (p->line, db->fp) == EOF) {
				return -1;
			}
			if (putc ('\n', db->fp) == EOF) {
				return -1;
			}
		}
	}
	return 0;
}