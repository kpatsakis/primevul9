static void add_one_entry_nis (struct commonio_db *db,
                               /*@owned@*/struct commonio_entry *newp)
{
	struct commonio_entry *p;

	for (p = db->head; NULL != p; p = p->next) {
		if (name_is_nis (p->eptr ? db->ops->getname (p->eptr)
		                         : p->line)) {
			/*@-mustfreeonly@*/
			newp->next = p;
			newp->prev = p->prev;
			/*@=mustfreeonly@*/
			if (NULL != p->prev) {
				p->prev->next = newp;
			} else {
				db->head = newp;
			}
			p->prev = newp;
			return;
		}
	}
	add_one_entry (db, newp);
}