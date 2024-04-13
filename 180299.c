_zip_dirent_write(struct zip_dirent *zde, FILE *fp, int localp,
		  struct zip_error *error)
{
    unsigned short dostime, dosdate;

    fwrite(localp ? LOCAL_MAGIC : CENTRAL_MAGIC, 1, 4, fp);

    if (!localp)
	_zip_write2(zde->version_madeby, fp);
    _zip_write2(zde->version_needed, fp);
    _zip_write2(zde->bitflags, fp);
    _zip_write2(zde->comp_method, fp);

    _zip_u2d_time(zde->last_mod, &dostime, &dosdate);
    _zip_write2(dostime, fp);
    _zip_write2(dosdate, fp);
    
    _zip_write4(zde->crc, fp);
    _zip_write4(zde->comp_size, fp);
    _zip_write4(zde->uncomp_size, fp);
    
    _zip_write2(zde->filename_len, fp);
    _zip_write2(zde->extrafield_len, fp);
    
    if (!localp) {
	_zip_write2(zde->comment_len, fp);
	_zip_write2(zde->disk_number, fp);
	_zip_write2(zde->int_attrib, fp);
	_zip_write4(zde->ext_attrib, fp);
	_zip_write4(zde->offset, fp);
    }

    if (zde->filename_len)
	fwrite(zde->filename, 1, zde->filename_len, fp);

    if (zde->extrafield_len)
	fwrite(zde->extrafield, 1, zde->extrafield_len, fp);

    if (!localp) {
	if (zde->comment_len)
	    fwrite(zde->comment, 1, zde->comment_len, fp);
    }

    if (ferror(fp)) {
	_zip_error_set(error, ZIP_ER_WRITE, errno);
	return -1;
    }

    return 0;
}