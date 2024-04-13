_zip_dirent_read(struct zip_dirent *zde, FILE *fp,
		 unsigned char **bufp, zip_uint32_t *leftp, int local,
		 struct zip_error *error)
{
    unsigned char buf[CDENTRYSIZE];
    unsigned char *cur;
    unsigned short dostime, dosdate;
    zip_uint32_t size;

    if (local)
	size = LENTRYSIZE;
    else
	size = CDENTRYSIZE;

    if (leftp && (*leftp < size)) {
	_zip_error_set(error, ZIP_ER_NOZIP, 0);
	return -1;
    }

    if (bufp) {
	/* use data from buffer */
	cur = *bufp;
    }
    else {
	/* read entry from disk */
	if ((fread(buf, 1, size, fp)<size)) {
	    _zip_error_set(error, ZIP_ER_READ, errno);
	    return -1;
	}
	cur = buf;
    }

    if (memcmp(cur, (local ? LOCAL_MAGIC : CENTRAL_MAGIC), 4) != 0) {
	_zip_error_set(error, ZIP_ER_NOZIP, 0);
	return -1;
    }
    cur += 4;

    
    /* convert buffercontents to zip_dirent */
    
    if (!local)
	zde->version_madeby = _zip_read2(&cur);
    else
	zde->version_madeby = 0;
    zde->version_needed = _zip_read2(&cur);
    zde->bitflags = _zip_read2(&cur);
    zde->comp_method = _zip_read2(&cur);
    
    /* convert to time_t */
    dostime = _zip_read2(&cur);
    dosdate = _zip_read2(&cur);
    zde->last_mod = _zip_d2u_time(dostime, dosdate);
    
    zde->crc = _zip_read4(&cur);
    zde->comp_size = _zip_read4(&cur);
    zde->uncomp_size = _zip_read4(&cur);
    
    zde->filename_len = _zip_read2(&cur);
    zde->extrafield_len = _zip_read2(&cur);
    
    if (local) {
	zde->comment_len = 0;
	zde->disk_number = 0;
	zde->int_attrib = 0;
	zde->ext_attrib = 0;
	zde->offset = 0;
    } else {
	zde->comment_len = _zip_read2(&cur);
	zde->disk_number = _zip_read2(&cur);
	zde->int_attrib = _zip_read2(&cur);
	zde->ext_attrib = _zip_read4(&cur);
	zde->offset = _zip_read4(&cur);
    }

    zde->filename = NULL;
    zde->extrafield = NULL;
    zde->comment = NULL;

    size += zde->filename_len+zde->extrafield_len+zde->comment_len;

    if (leftp && (*leftp < size)) {
	_zip_error_set(error, ZIP_ER_NOZIP, 0);
	return -1;
    }

    if (bufp) {
	if (zde->filename_len) {
	    zde->filename = _zip_readstr(&cur, zde->filename_len, 1, error);
	    if (!zde->filename)
		    return -1;
	}

	if (zde->extrafield_len) {
	    zde->extrafield = _zip_readstr(&cur, zde->extrafield_len, 0,
					   error);
	    if (!zde->extrafield)
		return -1;
	}

	if (zde->comment_len) {
	    zde->comment = _zip_readstr(&cur, zde->comment_len, 0, error);
	    if (!zde->comment)
		return -1;
	}
    }
    else {
	if (zde->filename_len) {
	    zde->filename = _zip_readfpstr(fp, zde->filename_len, 1, error);
	    if (!zde->filename)
		    return -1;
	}

	if (zde->extrafield_len) {
	    zde->extrafield = _zip_readfpstr(fp, zde->extrafield_len, 0,
					     error);
	    if (!zde->extrafield)
		return -1;
	}

	if (zde->comment_len) {
	    zde->comment = _zip_readfpstr(fp, zde->comment_len, 0, error);
	    if (!zde->comment)
		return -1;
	}
    }

    if (bufp)
      *bufp = cur;
    if (leftp)
	*leftp -= size;

    return 0;
}