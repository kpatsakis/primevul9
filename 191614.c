_zip_dirent_process_ef_utf_8(const zip_dirent_t *de, zip_uint16_t id, zip_string_t *str)
{
    zip_uint16_t ef_len;
    zip_uint32_t ef_crc;
    zip_buffer_t *buffer;

    const zip_uint8_t *ef = _zip_ef_get_by_id(de->extra_fields, &ef_len, id, 0, ZIP_EF_BOTH, NULL);

    if (ef == NULL || ef_len < 5 || ef[0] != 1) {
	return str;
    }

    if ((buffer = _zip_buffer_new((zip_uint8_t *)ef, ef_len)) == NULL) {
        return str;
    }

    _zip_buffer_get_8(buffer);
    ef_crc = _zip_buffer_get_32(buffer);

    if (_zip_string_crc32(str) == ef_crc) {
        zip_uint16_t len = (zip_uint16_t)_zip_buffer_left(buffer);
        zip_string_t *ef_str = _zip_string_new(_zip_buffer_get(buffer, len), len, ZIP_FL_ENC_UTF_8, NULL);

	if (ef_str != NULL) {
	    _zip_string_free(str);
	    str = ef_str;
	}
    }

    _zip_buffer_free(buffer);

    return str;
}