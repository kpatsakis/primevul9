static int ldb_canonicalise_utctime(struct ldb_context *ldb, void *mem_ctx,
				    const struct ldb_val *in, struct ldb_val *out)
{
	time_t t;
	int ret;
	ret = ldb_val_to_time(in, &t);
	if (ret != LDB_SUCCESS) {
		return ret;
	}
	out->data = (uint8_t *)ldb_timestring_utc(mem_ctx, t);
	if (out->data == NULL) {
		ldb_oom(ldb);
		return LDB_ERR_OPERATIONS_ERROR;
	}
	out->length = strlen((char *)out->data);
	return 0;
}