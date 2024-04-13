static int hdr_validate_areas(struct crypt_device *cd, json_object *hdr_jobj)
{
	struct interval *intervals;
	json_object *jobj_keyslots, *jobj_offset, *jobj_length, *jobj_segments, *jobj_area;
	int length, ret, i = 0;
	uint64_t metadata_size;

	if (!json_object_object_get_ex(hdr_jobj, "keyslots", &jobj_keyslots))
		return 1;

	/* segments are already validated */
	if (!json_object_object_get_ex(hdr_jobj, "segments", &jobj_segments))
		return 1;

	/* config is already validated */
	metadata_size = LUKS2_metadata_size_jobj(hdr_jobj);

	length = json_object_object_length(jobj_keyslots);

	/* Empty section */
	if (length == 0)
		return 0;

	if (length < 0) {
		log_dbg(cd, "Invalid keyslot areas specification.");
		return 1;
	}

	intervals = malloc(length * sizeof(*intervals));
	if (!intervals) {
		log_dbg(cd, "Not enough memory.");
		return -ENOMEM;
	}

	json_object_object_foreach(jobj_keyslots, key, val) {

		if (!(jobj_area = json_contains(cd, val, key, "Keyslot", "area", json_type_object)) ||
		    !json_contains(cd, jobj_area, key, "Keyslot area", "type", json_type_string) ||
		    !(jobj_offset = json_contains(cd, jobj_area, key, "Keyslot", "offset", json_type_string)) ||
		    !(jobj_length = json_contains(cd, jobj_area, key, "Keyslot", "size", json_type_string)) ||
		    !numbered(cd, "offset", json_object_get_string(jobj_offset)) ||
		    !numbered(cd, "size", json_object_get_string(jobj_length))) {
			free(intervals);
			return 1;
		}

		/* rule out values > UINT64_MAX */
		if (!json_str_to_uint64(jobj_offset, &intervals[i].offset) ||
		    !json_str_to_uint64(jobj_length, &intervals[i].length)) {
			free(intervals);
			return 1;
		}

		i++;
	}

	if (length != i) {
		free(intervals);
		return 1;
	}

	ret = validate_intervals(cd, length, intervals, metadata_size, LUKS2_hdr_and_areas_size_jobj(hdr_jobj)) ? 0 : 1;

	free(intervals);

	return ret;
}