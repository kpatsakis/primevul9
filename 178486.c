static int ldb_canonicalise_Boolean(struct ldb_context *ldb, void *mem_ctx,
			     const struct ldb_val *in, struct ldb_val *out)
{
	if (in->length >= 4 && strncasecmp((char *)in->data, "TRUE", in->length) == 0) {
		out->data = (uint8_t *)talloc_strdup(mem_ctx, "TRUE");
		out->length = 4;
	} else if (in->length >= 5 && strncasecmp((char *)in->data, "FALSE", in->length) == 0) {
		out->data = (uint8_t *)talloc_strdup(mem_ctx, "FALSE");
		out->length = 5;
	} else {
		return -1;
	}
	return 0;
}