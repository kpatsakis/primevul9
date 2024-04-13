void commonio_del_entry (struct commonio_db *db, const struct commonio_entry *p)
{
	if (p == db->cursor) {
		db->cursor = p->next;
	}

	if (NULL != p->prev) {
		p->prev->next = p->next;
	} else {
		db->head = p->next;
	}

	if (NULL != p->next) {
		p->next->prev = p->prev;
	} else {
		db->tail = p->prev;
	}

	db->changed = true;
}