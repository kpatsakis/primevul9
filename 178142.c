static void hdr_dump_segments(struct crypt_device *cd, json_object *hdr_jobj)
{
	char segment[16];
	json_object *jobj_segments, *jobj_segment, *jobj1, *jobj2;
	int i, j, flags;
	uint64_t value;

	log_std(cd, "Data segments:\n");
	json_object_object_get_ex(hdr_jobj, "segments", &jobj_segments);

	for (i = 0; i < LUKS2_SEGMENT_MAX; i++) {
		(void) snprintf(segment, sizeof(segment), "%i", i);
		if (!json_object_object_get_ex(jobj_segments, segment, &jobj_segment))
			continue;

		json_object_object_get_ex(jobj_segment, "type", &jobj1);
		log_std(cd, "  %s: %s\n", segment, json_object_get_string(jobj1));

		json_object_object_get_ex(jobj_segment, "offset", &jobj1);
		json_str_to_uint64(jobj1, &value);
		log_std(cd, "\toffset: %" PRIu64 " [bytes]\n", value);

		json_object_object_get_ex(jobj_segment, "size", &jobj1);
		if (!(strcmp(json_object_get_string(jobj1), "dynamic")))
			log_std(cd, "\tlength: (whole device)\n");
		else {
			json_str_to_uint64(jobj1, &value);
			log_std(cd, "\tlength: %" PRIu64 " [bytes]\n", value);
		}

		if (json_object_object_get_ex(jobj_segment, "encryption", &jobj1))
			log_std(cd, "\tcipher: %s\n", json_object_get_string(jobj1));

		if (json_object_object_get_ex(jobj_segment, "sector_size", &jobj1))
			log_std(cd, "\tsector: %" PRIu32 " [bytes]\n", crypt_jobj_get_uint32(jobj1));

		if (json_object_object_get_ex(jobj_segment, "integrity", &jobj1) &&
		    json_object_object_get_ex(jobj1, "type", &jobj2))
			log_std(cd, "\tintegrity: %s\n", json_object_get_string(jobj2));

		if (json_object_object_get_ex(jobj_segment, "flags", &jobj1) &&
		    (flags = (int)json_object_array_length(jobj1)) > 0) {
			jobj2 = json_object_array_get_idx(jobj1, 0);
			log_std(cd, "\tflags : %s", json_object_get_string(jobj2));
			for (j = 1; j < flags; j++) {
				jobj2 = json_object_array_get_idx(jobj1, j);
				log_std(cd, ", %s", json_object_get_string(jobj2));
			}
			log_std(cd, "\n");
		}

		log_std(cd, "\n");
	}
}