static void add_one_entry (struct commonio_db *db,
                           /*@owned@*/struct commonio_entry *p)
{
	/*@-mustfreeonly@*/
	p->next = NULL;
	p->prev = db->tail;
	/*@=mustfreeonly@*/
	if (NULL == db->head) {
		db->head = p;
	}
	if (NULL != db->tail) {
		db->tail->next = p;
	}
	db->tail = p;
}