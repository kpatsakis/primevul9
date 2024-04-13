json_object *json_contains(struct crypt_device *cd, json_object *jobj, const char *name,
			   const char *section, const char *key, json_type type)
{
	json_object *sobj;

	if (!json_object_object_get_ex(jobj, key, &sobj) ||
	    !json_object_is_type(sobj, type)) {
		log_dbg(cd, "%s \"%s\" is missing \"%s\" (%s) specification.",
			section, name, key, json_type_to_name(type));
		return NULL;
	}

	return sobj;
}