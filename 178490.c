static int ldb_index_format_Integer(struct ldb_context *ldb,
				    void *mem_ctx,
				    const struct ldb_val *in,
				    struct ldb_val *out)
{
	int64_t i;
	int ret;
	char prefix;
	size_t len;

	ret = val_to_int64(in, &i);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	if (i < 0) {
		/*
		 * i is negative, so this is subtraction rather than
		 * wrap-around.
		 */
		prefix = 'n';
		i = INT64_MAX + i + 1;
	} else if (i > 0) {
		prefix = 'p';
	} else {
		prefix = 'o';
	}

	out->data = (uint8_t *) talloc_asprintf(mem_ctx, "%c%019lld", prefix, (long long)i);
	if (out->data == NULL) {
		ldb_oom(ldb);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	len = talloc_array_length(out->data) - 1;
	if (len != 20) {
		ldb_debug(ldb, LDB_DEBUG_ERROR,
			  __location__ ": expected index format str %s to"
			  " have length 20 but got %zu",
			  (char*)out->data, len);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	out->length = 20;
	return 0;
}