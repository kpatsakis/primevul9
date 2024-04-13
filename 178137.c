int LUKS2_get_volume_key_size(struct luks2_hdr *hdr, int segment)
{
	json_object *jobj_digests, *jobj_digest_segments, *jobj_digest_keyslots, *jobj1;
	char buf[16];

	if (segment == CRYPT_DEFAULT_SEGMENT)
		segment = LUKS2_get_default_segment(hdr);

	if (snprintf(buf, sizeof(buf), "%u", segment) < 1)
		return -1;

	json_object_object_get_ex(hdr->jobj, "digests", &jobj_digests);

	json_object_object_foreach(jobj_digests, key, val) {
		UNUSED(key);
		json_object_object_get_ex(val, "segments", &jobj_digest_segments);
		json_object_object_get_ex(val, "keyslots", &jobj_digest_keyslots);

		if (!LUKS2_array_jobj(jobj_digest_segments, buf))
			continue;
		if (json_object_array_length(jobj_digest_keyslots) <= 0)
			continue;

		jobj1 = json_object_array_get_idx(jobj_digest_keyslots, 0);

		return LUKS2_keyslot_get_volume_key_size(hdr, json_object_get_string(jobj1));
	}

	return -1;
}