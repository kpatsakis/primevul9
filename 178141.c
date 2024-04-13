uint64_t LUKS2_get_data_offset(struct luks2_hdr *hdr)
{
	crypt_reencrypt_info ri;
	json_object *jobj;

	ri = LUKS2_reencrypt_status(hdr);
	if (ri == CRYPT_REENCRYPT_CLEAN || ri == CRYPT_REENCRYPT_CRASH) {
		jobj = LUKS2_get_segment_by_flag(hdr, "backup-final");
		if (jobj)
			return json_segment_get_offset(jobj, 1);
	}

	return json_segments_get_minimal_offset(LUKS2_get_segments_jobj(hdr), 1);
}