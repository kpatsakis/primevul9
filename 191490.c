int commonio_sort_wrt (struct commonio_db *shadow,
                       const struct commonio_db *passwd)
{
	struct commonio_entry *head = NULL, *pw_ptr, *spw_ptr;
	const char *name;

	if ((NULL == shadow) || (NULL == shadow->head)) {
		return 0;
	}

	for (pw_ptr = passwd->head; NULL != pw_ptr; pw_ptr = pw_ptr->next) {
		if (NULL == pw_ptr->eptr) {
			continue;
		}
		name = passwd->ops->getname (pw_ptr->eptr);
		for (spw_ptr = shadow->head;
		     NULL != spw_ptr;
		     spw_ptr = spw_ptr->next) {
			if (NULL == spw_ptr->eptr) {
				continue;
			}
			if (strcmp (name, shadow->ops->getname (spw_ptr->eptr))
			    == 0) {
				break;
			}
		}
		if (NULL == spw_ptr) {
			continue;
		}
		commonio_del_entry (shadow, spw_ptr);
		spw_ptr->next = head;
		head = spw_ptr;
	}

	for (spw_ptr = head; NULL != spw_ptr; spw_ptr = head) {
		head = head->next;

		if (NULL != shadow->head) {
			shadow->head->prev = spw_ptr;
		}
		spw_ptr->next = shadow->head;
		shadow->head = spw_ptr;
	}

	shadow->head->prev = NULL;
	shadow->changed = true;

	return 0;
}