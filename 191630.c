_zip_dirent_write(zip_t *za, zip_dirent_t *de, zip_flags_t flags)
{
    zip_uint16_t dostime, dosdate;
    zip_encoding_type_t com_enc, name_enc;
    zip_extra_field_t *ef;
    zip_extra_field_t *ef64;
    zip_uint32_t ef_total_size;
    bool is_zip64;
    bool is_really_zip64;
    zip_uint8_t buf[CDENTRYSIZE];
    zip_buffer_t *buffer;

    ef = NULL;

    name_enc = _zip_guess_encoding(de->filename, ZIP_ENCODING_UNKNOWN);
    com_enc = _zip_guess_encoding(de->comment, ZIP_ENCODING_UNKNOWN);

    if ((name_enc == ZIP_ENCODING_UTF8_KNOWN  && com_enc == ZIP_ENCODING_ASCII) ||
	(name_enc == ZIP_ENCODING_ASCII && com_enc == ZIP_ENCODING_UTF8_KNOWN) ||
	(name_enc == ZIP_ENCODING_UTF8_KNOWN  && com_enc == ZIP_ENCODING_UTF8_KNOWN))
	de->bitflags |= ZIP_GPBF_ENCODING_UTF_8;
    else {
	de->bitflags &= (zip_uint16_t)~ZIP_GPBF_ENCODING_UTF_8;
	if (name_enc == ZIP_ENCODING_UTF8_KNOWN) {
	    ef = _zip_ef_utf8(ZIP_EF_UTF_8_NAME, de->filename, &za->error);
	    if (ef == NULL)
		return -1;
	}
	if ((flags & ZIP_FL_LOCAL) == 0 && com_enc == ZIP_ENCODING_UTF8_KNOWN){
	    zip_extra_field_t *ef2 = _zip_ef_utf8(ZIP_EF_UTF_8_COMMENT, de->comment, &za->error);
	    if (ef2 == NULL) {
		_zip_ef_free(ef);
		return -1;
	    }
	    ef2->next = ef;
	    ef = ef2;
	}
    }

    is_really_zip64 = _zip_dirent_needs_zip64(de, flags);
    is_zip64 = (flags & (ZIP_FL_LOCAL|ZIP_FL_FORCE_ZIP64)) == (ZIP_FL_LOCAL|ZIP_FL_FORCE_ZIP64) || is_really_zip64;

    if (is_zip64) {
        zip_uint8_t ef_zip64[EFZIP64SIZE];
        zip_buffer_t *ef_buffer = _zip_buffer_new(ef_zip64, sizeof(ef_zip64));
        if (ef_buffer == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	    _zip_ef_free(ef);
            return -1;
        }

        if (flags & ZIP_FL_LOCAL) {
            if ((flags & ZIP_FL_FORCE_ZIP64) || de->comp_size > ZIP_UINT32_MAX || de->uncomp_size > ZIP_UINT32_MAX) {
                _zip_buffer_put_64(ef_buffer, de->uncomp_size);
                _zip_buffer_put_64(ef_buffer, de->comp_size);
            }
        }
        else {
            if ((flags & ZIP_FL_FORCE_ZIP64) || de->comp_size > ZIP_UINT32_MAX || de->uncomp_size > ZIP_UINT32_MAX || de->offset > ZIP_UINT32_MAX) {
                if (de->uncomp_size >= ZIP_UINT32_MAX) {
                    _zip_buffer_put_64(ef_buffer, de->uncomp_size);
                }
                if (de->comp_size >= ZIP_UINT32_MAX) {
                    _zip_buffer_put_64(ef_buffer, de->comp_size);
                }
                if (de->offset >= ZIP_UINT32_MAX) {
                    _zip_buffer_put_64(ef_buffer, de->offset);
                }
            }
        }

        if (!_zip_buffer_ok(ef_buffer)) {
            zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
            _zip_buffer_free(ef_buffer);
	    _zip_ef_free(ef);
            return -1;
        }

        ef64 = _zip_ef_new(ZIP_EF_ZIP64, (zip_uint16_t)(_zip_buffer_offset(ef_buffer)), ef_zip64, ZIP_EF_BOTH);
        _zip_buffer_free(ef_buffer);
        ef64->next = ef;
        ef = ef64;
    }

    if ((buffer = _zip_buffer_new(buf, sizeof(buf))) == NULL) {
        zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
        _zip_ef_free(ef);
        return -1;
    }

    _zip_buffer_put(buffer, (flags & ZIP_FL_LOCAL) ? LOCAL_MAGIC : CENTRAL_MAGIC, 4);

    if ((flags & ZIP_FL_LOCAL) == 0) {
        _zip_buffer_put_16(buffer, (zip_uint16_t)(is_really_zip64 ? 45 : de->version_madeby));
    }
    _zip_buffer_put_16(buffer, (zip_uint16_t)(is_really_zip64 ? 45 : de->version_needed));
    _zip_buffer_put_16(buffer, de->bitflags&0xfff9); /* clear compression method specific flags */
    _zip_buffer_put_16(buffer, (zip_uint16_t)de->comp_method);

    _zip_u2d_time(de->last_mod, &dostime, &dosdate);
    _zip_buffer_put_16(buffer, dostime);
    _zip_buffer_put_16(buffer, dosdate);

    _zip_buffer_put_32(buffer, de->crc);

    if (((flags & ZIP_FL_LOCAL) == ZIP_FL_LOCAL) && ((de->comp_size >= ZIP_UINT32_MAX) || (de->uncomp_size >= ZIP_UINT32_MAX))) {
	/* In local headers, if a ZIP64 EF is written, it MUST contain
	 * both compressed and uncompressed sizes (even if one of the
	 * two is smaller than 0xFFFFFFFF); on the other hand, those
	 * may only appear when the corresponding standard entry is
	 * 0xFFFFFFFF.  (appnote.txt 4.5.3) */
	_zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
	_zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
    }
    else {
        if (de->comp_size < ZIP_UINT32_MAX) {
	    _zip_buffer_put_32(buffer, (zip_uint32_t)de->comp_size);
        }
        else {
	    _zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
        }
        if (de->uncomp_size < ZIP_UINT32_MAX) {
	    _zip_buffer_put_32(buffer, (zip_uint32_t)de->uncomp_size);
        }
        else {
	    _zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
        }
    }

    _zip_buffer_put_16(buffer, _zip_string_length(de->filename));
    /* TODO: check for overflow */
    ef_total_size = (zip_uint32_t)_zip_ef_size(de->extra_fields, flags) + (zip_uint32_t)_zip_ef_size(ef, ZIP_EF_BOTH);
    _zip_buffer_put_16(buffer, (zip_uint16_t)ef_total_size);

    if ((flags & ZIP_FL_LOCAL) == 0) {
	_zip_buffer_put_16(buffer, _zip_string_length(de->comment));
	_zip_buffer_put_16(buffer, (zip_uint16_t)de->disk_number);
	_zip_buffer_put_16(buffer, de->int_attrib);
	_zip_buffer_put_32(buffer, de->ext_attrib);
	if (de->offset < ZIP_UINT32_MAX)
	    _zip_buffer_put_32(buffer, (zip_uint32_t)de->offset);
	else
	    _zip_buffer_put_32(buffer, ZIP_UINT32_MAX);
    }

    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(&za->error, ZIP_ER_INTERNAL, 0);
        _zip_buffer_free(buffer);
        _zip_ef_free(ef);
        return -1;
    }

    if (_zip_write(za, buf, _zip_buffer_offset(buffer)) < 0) {
        _zip_buffer_free(buffer);
        _zip_ef_free(ef);
        return -1;
    }

    _zip_buffer_free(buffer);

    if (de->filename) {
	if (_zip_string_write(za, de->filename) < 0) {
            _zip_ef_free(ef);
	    return -1;
	}
    }

    if (ef) {
	if (_zip_ef_write(za, ef, ZIP_EF_BOTH) < 0) {
            _zip_ef_free(ef);
	    return -1;
	}
    }
    _zip_ef_free(ef);
    if (de->extra_fields) {
	if (_zip_ef_write(za, de->extra_fields, flags) < 0) {
	    return -1;
	}
    }

    if ((flags & ZIP_FL_LOCAL) == 0) {
	if (de->comment) {
	    if (_zip_string_write(za, de->comment) < 0) {
		return -1;
	    }
	}
    }


    return is_zip64;
}