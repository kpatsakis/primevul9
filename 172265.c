static char *lsqlite3_tprintf(TALLOC_CTX *mem_ctx, const char *fmt, ...)
{
	char *str, *ret;
	va_list ap;

	va_start(ap, fmt);
        str = sqlite3_vmprintf(fmt, ap);
	va_end(ap);

	if (str == NULL) return NULL;

	ret = talloc_strdup(mem_ctx, str);
	if (ret == NULL) {
		sqlite3_free(str);
		return NULL;
	}

	sqlite3_free(str);
	return ret;
}