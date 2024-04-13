static bool check_alias(GSList *aliases, char *name)
{
	GSList *list;

	if (aliases) {
		for (list = aliases; list; list = list->next) {
			int len = strlen((char *)list->data);
			if (strncmp((char *)list->data, name, len) == 0)
				return true;
		}
	}

	return false;
}