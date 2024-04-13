/*@observer@*/ /*@null@*/const void *commonio_next (struct commonio_db *db)
{
	void *eptr;

	if (!db->isopen) {
		errno = EINVAL;
		return 0;
	}
	if (NULL == db->cursor) {
		db->cursor = db->head;
	} else {
		db->cursor = db->cursor->next;
	}

	while (NULL != db->cursor) {
		eptr = db->cursor->eptr;
		if (NULL != eptr) {
			return eptr;
		}

		db->cursor = db->cursor->next;
	}
	return NULL;
}