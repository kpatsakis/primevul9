static struct elevator_type *elevator_find(const char *name)
{
	struct elevator_type *e;

	list_for_each_entry(e, &elv_list, list) {
		if (elevator_match(e, name))
			return e;
	}

	return NULL;
}