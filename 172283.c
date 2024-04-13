static int lsql_add(struct lsql_context *ctx)
{
	struct ldb_module *module = ctx->module;
	struct ldb_request *req = ctx->req;
	struct lsqlite3_private *lsqlite3;
	struct ldb_context *ldb;
	struct ldb_message *msg = req->op.add.message;
        long long eid;
	char *dn, *ndn;
	char *errmsg;
	char *query;
	unsigned int i;
	int ret;

	ldb = ldb_module_get_ctx(module);
	lsqlite3 = talloc_get_type(ldb_module_get_private(module),
				   struct lsqlite3_private);

        /* See if this is an ltdb special */
	if (ldb_dn_is_special(msg->dn)) {
/*
		struct ldb_dn *c;
		c = ldb_dn_new(local_ctx, ldb, "@INDEXLIST");
		if (ldb_dn_compare(ldb, msg->dn, c) == 0) {
#warning "should we handle indexes somehow ?"
			ret = LDB_ERR_UNWILLING_TO_PERFORM;
			goto done;
		}
*/
                /* Others return an error */
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	/* create linearized and normalized dns */
	dn = ldb_dn_alloc_linearized(ctx, msg->dn);
	ndn = ldb_dn_alloc_casefold(ctx, msg->dn);
	if (dn == NULL || ndn == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	query = lsqlite3_tprintf(ctx,
				   /* Add new entry */
				   "INSERT OR ABORT INTO ldb_entry "
				   "('dn', 'norm_dn') "
				   "VALUES ('%q', '%q');",
				dn, ndn);
	if (query == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = sqlite3_exec(lsqlite3->sqlite, query, NULL, NULL, &errmsg);
	if (ret != SQLITE_OK) {
		if (errmsg) {
			ldb_set_errstring(ldb, errmsg);
			free(errmsg);
		}
		return LDB_ERR_OPERATIONS_ERROR;
	}

	eid = lsqlite3_get_eid_ndn(lsqlite3->sqlite, ctx, ndn);
	if (eid == -1) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	for (i = 0; i < msg->num_elements; i++) {
		const struct ldb_message_element *el = &msg->elements[i];
		const struct ldb_schema_attribute *a;
		char *attr;
		unsigned int j;

		/* Get a case-folded copy of the attribute name */
		attr = ldb_attr_casefold(ctx, el->name);
		if (attr == NULL) {
			return LDB_ERR_OPERATIONS_ERROR;
		}

		a = ldb_schema_attribute_by_name(ldb, el->name);

		if (el->num_value == 0) {
			ldb_asprintf_errstring(ldb, "attribute %s on %s specified, but with 0 values (illegal)",
					       el->name, ldb_dn_get_linearized(msg->dn));
			return LDB_ERR_CONSTRAINT_VIOLATION;
		}

		/* For each value of the specified attribute name... */
		for (j = 0; j < el->num_values; j++) {
			struct ldb_val value;
			char *insert;

			/* Get a canonicalised copy of the data */
			a->syntax->canonicalise_fn(ldb, ctx, &(el->values[j]), &value);
			if (value.data == NULL) {
				return LDB_ERR_OPERATIONS_ERROR;
			}

			insert = lsqlite3_tprintf(ctx,
					"INSERT OR ROLLBACK INTO ldb_attribute_values "
					"('eid', 'attr_name', 'norm_attr_name',"
					" 'attr_value', 'norm_attr_value') "
					"VALUES ('%lld', '%q', '%q', '%q', '%q');",
					eid, el->name, attr,
					el->values[j].data, value.data);
			if (insert == NULL) {
				return LDB_ERR_OPERATIONS_ERROR;
			}

			ret = sqlite3_exec(lsqlite3->sqlite, insert, NULL, NULL, &errmsg);
			if (ret != SQLITE_OK) {
				if (errmsg) {
					ldb_set_errstring(ldb, errmsg);
					free(errmsg);
				}
				return LDB_ERR_OPERATIONS_ERROR;
			}
		}
	}

	return LDB_SUCCESS;
}