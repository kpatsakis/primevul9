static json_bool validate_keyslots_array(struct crypt_device *cd,
					 json_object *jarr, json_object *jobj_keys)
{
	json_object *jobj;
	int i = 0, length = (int) json_object_array_length(jarr);

	while (i < length) {
		jobj = json_object_array_get_idx(jarr, i);
		if (!json_object_is_type(jobj, json_type_string)) {
			log_dbg(cd, "Illegal value type in keyslots array at index %d.", i);
			return 0;
		}

		if (!json_contains(cd, jobj_keys, "", "Keyslots section",
				   json_object_get_string(jobj), json_type_object))
			return 0;

		i++;
	}

	return 1;
}