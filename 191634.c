_zip_ef_utf8(zip_uint16_t id, zip_string_t *str, zip_error_t *error)
{
    const zip_uint8_t *raw;
    zip_uint32_t len;
    zip_buffer_t *buffer;
    zip_extra_field_t *ef;

    if ((raw=_zip_string_get(str, &len, ZIP_FL_ENC_RAW, NULL)) == NULL) {
	/* error already set */
	return NULL;
    }

    if (len+5 > ZIP_UINT16_MAX) {
        zip_error_set(error, ZIP_ER_INVAL, 0); /* TODO: better error code? */
        return NULL;
    }

    if ((buffer = _zip_buffer_new(NULL, len+5)) == NULL) {
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    _zip_buffer_put_8(buffer, 1);
    _zip_buffer_put_32(buffer, _zip_string_crc32(str));
    _zip_buffer_put(buffer, raw, len);

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        _zip_buffer_free(buffer);
        return NULL;
    }

    ef = _zip_ef_new(id, (zip_uint16_t)(_zip_buffer_offset(buffer)), _zip_buffer_data(buffer), ZIP_EF_BOTH);
    _zip_buffer_free(buffer);

    return ef;
}