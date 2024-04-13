_zip_readfpstr(FILE *fp, unsigned int len, int nulp, struct zip_error *error)
{
    char *r, *o;

    r = (char *)malloc(nulp ? len+1 : len);
    if (!r) {
	_zip_error_set(error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    if (fread(r, 1, len, fp)<len) {
	free(r);
	_zip_error_set(error, ZIP_ER_READ, errno);
	return NULL;
    }

    if (nulp) {
	/* replace any in-string NUL characters with spaces */
	r[len] = 0;
	for (o=r; o<r+len; o++)
	    if (*o == '\0')
		*o = ' ';
    }
    
    return r;
}