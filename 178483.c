static int ldb_comparison_Boolean(struct ldb_context *ldb, void *mem_ctx,
			   const struct ldb_val *v1, const struct ldb_val *v2)
{
	if (v1->length != v2->length) {
		return v1->length - v2->length;
	}
	return strncasecmp((char *)v1->data, (char *)v2->data, v1->length);
}