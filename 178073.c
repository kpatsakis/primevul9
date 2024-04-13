static int firebird_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, size_t unquotedlen, /* {{{ */
	char **quoted, size_t *quotedlen, enum pdo_param_type paramtype)
{
	int qcount = 0;
	char const *co, *l, *r;
	char *c;

	if (!unquotedlen) {
		*quotedlen = 2;
		*quoted = emalloc(*quotedlen+1);
		strcpy(*quoted, "''");
		return 1;
	}

	/* Firebird only requires single quotes to be doubled if string lengths are used */
	/* count the number of ' characters */
	for (co = unquoted; (co = strchr(co,'\'')); qcount++, co++);

	*quotedlen = unquotedlen + qcount + 2;
	*quoted = c = emalloc(*quotedlen+1);
	*c++ = '\'';

	/* foreach (chunk that ends in a quote) */
	for (l = unquoted; (r = strchr(l,'\'')); l = r+1) {
		strncpy(c, l, r-l+1);
		c += (r-l+1);
		/* add the second quote */
		*c++ = '\'';
	}

	/* copy the remainder */
	strncpy(c, l, *quotedlen-(c-*quoted)-1);
	(*quoted)[*quotedlen-1] = '\'';
	(*quoted)[*quotedlen]   = '\0';

	return 1;
}