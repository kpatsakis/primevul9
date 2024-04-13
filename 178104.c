uint64_t LUKS2_hdr_and_areas_size_jobj(json_object *jobj)
{
	return 2 * LUKS2_metadata_size_jobj(jobj) + LUKS2_keyslots_size_jobj(jobj);
}