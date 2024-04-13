static json_object *json_get_segments_jobj(json_object *hdr_jobj)
{
	json_object *jobj_segments;

	if (!hdr_jobj || !json_object_object_get_ex(hdr_jobj, "segments", &jobj_segments))
		return NULL;

	return jobj_segments;
}