_zip_cdir_write(struct zip_cdir *cd, FILE *fp, struct zip_error *error)
{
    int i;

    cd->offset = ftello(fp);

    for (i=0; i<cd->nentry; i++) {
	if (_zip_dirent_write(cd->entry+i, fp, 0, error) != 0)
	    return -1;
    }

    cd->size = ftello(fp) - cd->offset;
    
    /* clearerr(fp); */
    fwrite(EOCD_MAGIC, 1, 4, fp);
    _zip_write4(0, fp);
    _zip_write2((unsigned short)cd->nentry, fp);
    _zip_write2((unsigned short)cd->nentry, fp);
    _zip_write4(cd->size, fp);
    _zip_write4(cd->offset, fp);
    _zip_write2(cd->comment_len, fp);
    fwrite(cd->comment, 1, cd->comment_len, fp);

    if (ferror(fp)) {
	_zip_error_set(error, ZIP_ER_WRITE, errno);
	return -1;
    }

    return 0;
}