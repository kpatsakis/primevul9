int LUKS2_get_data_size(struct luks2_hdr *hdr, uint64_t *size, bool *dynamic)
{
	int sector_size;
	json_object *jobj_segments, *jobj_size;
	uint64_t tmp = 0;

	if (!size || !json_object_object_get_ex(hdr->jobj, "segments", &jobj_segments))
		return -EINVAL;

	json_object_object_foreach(jobj_segments, key, val) {
		UNUSED(key);
		if (json_segment_is_backup(val))
			continue;

		json_object_object_get_ex(val, "size", &jobj_size);
		if (!strcmp(json_object_get_string(jobj_size), "dynamic")) {
			sector_size = json_segment_get_sector_size(val);
			/* last dynamic segment must have at least one sector in size */
			if (tmp)
				*size = tmp + (sector_size > 0 ? sector_size : SECTOR_SIZE);
			else
				*size = 0;
			if (dynamic)
				*dynamic = true;
			return 0;
		}

		tmp += crypt_jobj_get_uint64(jobj_size);
	}

	/* impossible, real device size must not be zero */
	if (!tmp)
		return -EINVAL;

	*size = tmp;
	if (dynamic)
		*dynamic = false;
	return 0;
}