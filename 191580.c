void *ldb_get_opaque(struct ldb_context *ldb, const char *name)
{
	struct ldb_opaque *o;
	for (o=ldb->opaque;o;o=o->next) {
		if (strcmp(o->name, name) == 0) {
			return o->value;
		}
	}
	return NULL;
}