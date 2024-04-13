static void free_linked_list (struct commonio_db *db)
{
	struct commonio_entry *p;

	while (NULL != db->head) {
		p = db->head;
		db->head = p->next;

		if (NULL != p->line) {
			free (p->line);
		}

		if (NULL != p->eptr) {
			db->ops->free (p->eptr);
		}

		free (p);
	}
	db->tail = NULL;
}