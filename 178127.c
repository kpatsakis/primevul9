static json_bool segment_has_digest(const char *segment_name, json_object *jobj_digests)
{
	json_object *jobj_segments;

	json_object_object_foreach(jobj_digests, key, val) {
		UNUSED(key);
		json_object_object_get_ex(val, "segments", &jobj_segments);
		if (LUKS2_array_jobj(jobj_segments, segment_name))
			return 1;
	}

	return 0;
}