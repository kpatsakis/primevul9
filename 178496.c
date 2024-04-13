int ldb_handler_copy(struct ldb_context *ldb, void *mem_ctx,
		     const struct ldb_val *in, struct ldb_val *out)
{
	*out = ldb_val_dup(mem_ctx, in);
	if (in->length > 0 && out->data == NULL) {
		ldb_oom(ldb);
		return -1;
	}
	return 0;
}