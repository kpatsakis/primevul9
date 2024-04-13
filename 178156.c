int json_object_copy(json_object *jobj_src, json_object **jobj_dst)
{
	if (!jobj_src || !jobj_dst || *jobj_dst)
		return -1;

#if HAVE_DECL_JSON_OBJECT_DEEP_COPY
	return json_object_deep_copy(jobj_src, jobj_dst, NULL);
#else
	*jobj_dst = json_tokener_parse(json_object_get_string(jobj_src));
	return *jobj_dst ? 0 : -1;
#endif
}