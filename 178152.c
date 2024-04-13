static int hdr_validate_config(struct crypt_device *cd, json_object *hdr_jobj)
{
	json_object *jobj_config, *jobj, *jobj1;
	int i;
	uint64_t keyslots_size, metadata_size, segment_offset;

	if (!json_object_object_get_ex(hdr_jobj, "config", &jobj_config)) {
		log_dbg(cd, "Missing config section.");
		return 1;
	}

	if (!(jobj = json_contains(cd, jobj_config, "section", "Config", "json_size", json_type_string)) ||
	    !json_str_to_uint64(jobj, &metadata_size))
		return 1;

	/* single metadata instance is assembled from json area size plus
	 * binary header size */
	metadata_size += LUKS2_HDR_BIN_LEN;

	if (!(jobj = json_contains(cd, jobj_config, "section", "Config", "keyslots_size", json_type_string)) ||
	    !json_str_to_uint64(jobj, &keyslots_size))
		return 1;

	if (LUKS2_check_metadata_area_size(metadata_size)) {
		log_dbg(cd, "Unsupported LUKS2 header size (%" PRIu64 ").", metadata_size);
		return 1;
	}

	if (LUKS2_check_keyslots_area_size(keyslots_size)) {
		log_dbg(cd, "Unsupported LUKS2 keyslots size (%" PRIu64 ").", keyslots_size);
		return 1;
	}

	/*
	 * validate keyslots_size fits in between (2 * metadata_size) and first
	 * segment_offset (except detached header)
	 */
	segment_offset = json_segments_get_minimal_offset(json_get_segments_jobj(hdr_jobj), 0);
	if (segment_offset &&
	    (segment_offset < keyslots_size ||
	     (segment_offset - keyslots_size) < (2 * metadata_size))) {
		log_dbg(cd, "keyslots_size is too large %" PRIu64 " (bytes). Data offset: %" PRIu64
			", keyslots offset: %" PRIu64, keyslots_size, segment_offset, 2 * metadata_size);
		return 1;
	}

	/* Flags array is optional */
	if (json_object_object_get_ex(jobj_config, "flags", &jobj)) {
		if (!json_contains(cd, jobj_config, "section", "Config", "flags", json_type_array))
			return 1;

		/* All array members must be strings */
		for (i = 0; i < (int) json_object_array_length(jobj); i++)
			if (!json_object_is_type(json_object_array_get_idx(jobj, i), json_type_string))
				return 1;
	}

	/* Requirements object is optional */
	if (json_object_object_get_ex(jobj_config, "requirements", &jobj)) {
		if (!json_contains(cd, jobj_config, "section", "Config", "requirements", json_type_object))
			return 1;

		/* Mandatory array is optional */
		if (json_object_object_get_ex(jobj, "mandatory", &jobj1)) {
			if (!json_contains(cd, jobj, "section", "Requirements", "mandatory", json_type_array))
				return 1;

			/* All array members must be strings */
			for (i = 0; i < (int) json_object_array_length(jobj1); i++)
				if (!json_object_is_type(json_object_array_get_idx(jobj1, i), json_type_string))
					return 1;
		}
	}

	return 0;
}