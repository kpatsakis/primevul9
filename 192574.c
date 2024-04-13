coolkey_find_object_by_id(list_t *list, unsigned long object_id)
{
	int pos;
	static sc_cardctl_coolkey_object_t cmp = {{
		"", 0, 0, 0, SC_PATH_TYPE_DF_NAME,
		{ COOLKEY_AID, sizeof(COOLKEY_AID)-1 }
	}, 0, 0, NULL};

	cmp.id = object_id;
	if ((pos = list_locate(list, &cmp)) < 0)
		return NULL;

	return list_get_at(list, pos);
}