_zip_dirent_size(zip_source_t *src, zip_uint16_t flags, zip_error_t *error)
{
    zip_int32_t size;
    bool local = (flags & ZIP_EF_LOCAL) != 0;
    int i;
    zip_uint8_t b[6];
    zip_buffer_t *buffer;

    size = local ? LENTRYSIZE : CDENTRYSIZE;

    if (zip_source_seek(src, local ? 26 : 28, SEEK_CUR) < 0) {
        _zip_error_set_from_source(error, src);
	return -1;
    }

    if ((buffer = _zip_buffer_new_from_source(src, local ? 4 : 6, b, error)) == NULL) {
	return -1;
    }

    for (i=0; i<(local ? 2 : 3); i++) {
	size += _zip_buffer_get_16(buffer);
    }

    if (!_zip_buffer_eof(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
	_zip_buffer_free(buffer);
        return -1;
    }

    _zip_buffer_free(buffer);
    return size;
}