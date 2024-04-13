_zip_cdir_write(zip_t *za, const zip_filelist_t *filelist, zip_uint64_t survivors)
{
    zip_uint64_t offset, size;
    zip_string_t *comment;
    zip_uint8_t buf[EOCDLEN + EOCD64LEN + EOCD64LOCLEN];
    zip_buffer_t *buffer;
    zip_int64_t off;
    zip_uint64_t i;
    bool is_zip64;
    int ret;

    if ((off = zip_source_tell_write(za->src)) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return -1;
    }
    offset = (zip_uint64_t)off;

    is_zip64 = false;

    for (i=0; i<survivors; i++) {
	zip_entry_t *entry = za->entry+filelist[i].idx;

	if ((ret=_zip_dirent_write(za, entry->changes ? entry->changes : entry->orig, ZIP_FL_CENTRAL)) < 0)
	    return -1;
	if (ret)
	    is_zip64 = true;
    }

    if ((off = zip_source_tell_write(za->src)) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return -1;
    }
    size = (zip_uint64_t)off - offset;

    if (offset > ZIP_UINT32_MAX || survivors > ZIP_UINT16_MAX)
	is_zip64 = true;


    if ((buffer = _zip_buffer_new(buf, sizeof(buf))) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        return -1;
    }

    if (is_zip64) {
	_zip_buffer_put(buffer, EOCD64_MAGIC, 4);
        _zip_buffer_put_64(buffer, EOCD64LEN-12);
	_zip_buffer_put_16(buffer, 45);
	_zip_buffer_put_16(buffer, 45);
	_zip_buffer_put_32(buffer, 0);
	_zip_buffer_put_32(buffer, 0);
	_zip_buffer_put_64(buffer, survivors);
	_zip_buffer_put_64(buffer, survivors);
	_zip_buffer_put_64(buffer, size);
	_zip_buffer_put_64(buffer, offset);
	_zip_buffer_put(buffer, EOCD64LOC_MAGIC, 4);
	_zip_buffer_put_32(buffer, 0);
	_zip_buffer_put_64(buffer, offset+size);
	_zip_buffer_put_32(buffer, 1);
    }

    _zip_buffer_put(buffer, EOCD_MAGIC, 4);
    _zip_buffer_put_32(buffer, 0);
    _zip_buffer_put_16(buffer, (zip_uint16_t)(survivors >= ZIP_UINT16_MAX ? ZIP_UINT16_MAX : survivors));
    _zip_buffer_put_16(buffer, (zip_uint16_t)(survivors >= ZIP_UINT16_MAX ? ZIP_UINT16_MAX : survivors));
    _zip_buffer_put_32(buffer, size >= ZIP_UINT32_MAX ? ZIP_UINT32_MAX : (zip_uint32_t)size);
    _zip_buffer_put_32(buffer, offset >= ZIP_UINT32_MAX ? ZIP_UINT32_MAX : (zip_uint32_t)offset);

    comment = za->comment_changed ? za->comment_changes : za->comment_orig;

    _zip_buffer_put_16(buffer, (zip_uint16_t)(comment ? comment->length : 0));

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        _zip_buffer_free(buffer);
        return -1;
    }

    if (_zip_write(za, _zip_buffer_data(buffer), _zip_buffer_offset(buffer)) < 0) {
        _zip_buffer_free(buffer);
	return -1;
    }

    _zip_buffer_free(buffer);

    if (comment) {
	if (_zip_write(za, comment->raw, comment->length) < 0) {
	    return -1;
	}
    }

    return (zip_int64_t)size;
}