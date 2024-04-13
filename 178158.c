static int hdr_validate_digests(struct crypt_device *cd, json_object *hdr_jobj)
{
	json_object *jarr_keys, *jarr_segs, *jobj, *jobj_keyslots, *jobj_segments;

	if (!json_object_object_get_ex(hdr_jobj, "digests", &jobj)) {
		log_dbg(cd, "Missing digests section.");
		return 1;
	}

	/* keyslots are not yet validated, but we need to know digest doesn't reference missing keyslot */
	if (!json_object_object_get_ex(hdr_jobj, "keyslots", &jobj_keyslots))
		return 1;

	/* segments are not yet validated, but we need to know digest doesn't reference missing segment */
	if (!json_object_object_get_ex(hdr_jobj, "segments", &jobj_segments))
		return 1;

	json_object_object_foreach(jobj, key, val) {
		if (!numbered(cd, "Digest", key))
			return 1;

		if (!json_contains(cd, val, key, "Digest", "type", json_type_string) ||
		    !(jarr_keys = json_contains(cd, val, key, "Digest", "keyslots", json_type_array)) ||
		    !(jarr_segs = json_contains(cd, val, key, "Digest", "segments", json_type_array)))
			return 1;

		if (!validate_keyslots_array(cd, jarr_keys, jobj_keyslots))
			return 1;
		if (!validate_segments_array(cd, jarr_segs, jobj_segments))
			return 1;
	}

	return 0;
}