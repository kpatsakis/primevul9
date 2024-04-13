static const char *get_priority_desc(json_object *jobj)
{
	crypt_keyslot_priority priority;
	json_object *jobj_priority;
	const char *text;

	if (json_object_object_get_ex(jobj, "priority", &jobj_priority))
		priority = (crypt_keyslot_priority)(int)json_object_get_int(jobj_priority);
	else
		priority = CRYPT_SLOT_PRIORITY_NORMAL;

	switch (priority) {
		case CRYPT_SLOT_PRIORITY_IGNORE: text = "ignored"; break;
		case CRYPT_SLOT_PRIORITY_PREFER: text = "preferred"; break;
		case CRYPT_SLOT_PRIORITY_NORMAL: text = "normal"; break;
		default: text = "invalid";
	}

	return text;
}