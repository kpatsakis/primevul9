cdf_app_to_mime(const char *vbuf, const struct nv *nv)
{
	size_t i;
	const char *rv = NULL;
	char *old_lc_ctype;

	old_lc_ctype = setlocale(LC_CTYPE, NULL);
	assert(old_lc_ctype != NULL);
	old_lc_ctype = strdup(old_lc_ctype);
	assert(old_lc_ctype != NULL);
	(void)setlocale(LC_CTYPE, "C");
	for (i = 0; nv[i].pattern != NULL; i++)
		if (strcasestr(vbuf, nv[i].pattern) != NULL) {
			rv = nv[i].mime;
			break;
		}
	(void)setlocale(LC_CTYPE, old_lc_ctype);
	free(old_lc_ctype);
	return rv;
}