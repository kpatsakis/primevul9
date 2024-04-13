static const ut8 *parse_line_header_source(RzBinFile *bf, const ut8 *buf, const ut8 *buf_end, RzBinDwarfLineHeader *hdr) {
	RzPVector incdirs;
	rz_pvector_init(&incdirs, free);
	while (buf + 1 < buf_end) {
		size_t maxlen = RZ_MIN((size_t)(buf_end - buf) - 1, 0xfff);
		size_t len = rz_str_nlen((const char *)buf, maxlen);
		char *str = rz_str_ndup((const char *)buf, len);
		if (len < 1 || len >= 0xfff || !str) {
			buf += 1;
			free(str);
			break;
		}
		rz_pvector_push(&incdirs, str);
		buf += len + 1;
	}
	hdr->include_dirs_count = rz_pvector_len(&incdirs);
	hdr->include_dirs = (char **)rz_pvector_flush(&incdirs);
	rz_pvector_fini(&incdirs);

	RzVector file_names;
	rz_vector_init(&file_names, sizeof(RzBinDwarfLineFileEntry), NULL, NULL);
	while (buf + 1 < buf_end) {
		const char *filename = (const char *)buf;
		size_t maxlen = RZ_MIN((size_t)(buf_end - buf - 1), 0xfff);
		ut64 id_idx, mod_time, file_len;
		size_t len = rz_str_nlen(filename, maxlen);

		if (!len) {
			buf++;
			break;
		}
		buf += len + 1;
		if (buf >= buf_end) {
			buf = NULL;
			goto beach;
		}
		buf = rz_uleb128(buf, buf_end - buf, &id_idx, NULL);
		if (buf >= buf_end) {
			buf = NULL;
			goto beach;
		}
		buf = rz_uleb128(buf, buf_end - buf, &mod_time, NULL);
		if (buf >= buf_end) {
			buf = NULL;
			goto beach;
		}
		buf = rz_uleb128(buf, buf_end - buf, &file_len, NULL);
		if (buf >= buf_end) {
			buf = NULL;
			goto beach;
		}
		RzBinDwarfLineFileEntry *entry = rz_vector_push(&file_names, NULL);
		entry->name = strdup(filename);
		entry->id_idx = id_idx;
		entry->mod_time = mod_time;
		entry->file_len = file_len;
	}
	hdr->file_names_count = rz_vector_len(&file_names);
	hdr->file_names = rz_vector_flush(&file_names);
	rz_vector_fini(&file_names);

beach:
	return buf;
}