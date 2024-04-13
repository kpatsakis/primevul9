static int lsql_modify(struct lsql_context *ctx)
{
	struct ldb_module *module = ctx->module;
	struct ldb_request *req = ctx->req;
	struct lsqlite3_private *lsqlite3;
	struct ldb_context *ldb;
	struct ldb_message *msg = req->op.mod.message;
	long long eid;
	char *errmsg;
	unsigned int i;
	int ret;

	ldb = ldb_module_get_ctx(module);
	lsqlite3 = talloc_get_type(ldb_module_get_private(module),
				   struct lsqlite3_private);

        /* See if this is an ltdb special */
	if (ldb_dn_is_special(msg->dn)) {
                /* Others return an error */
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	eid = lsqlite3_get_eid(lsqlite3, msg->dn);
	if (eid == -1) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	for (i = 0; i < msg->num_elements; i++) {
		const struct ldb_message_element *el = &msg->elements[i];
		const struct ldb_schema_attribute *a;
		unsigned int flags = el->flags & LDB_FLAG_MOD_MASK;
		char *attr;
		char *mod;
		unsigned int j;

		/* Get a case-folded copy of the attribute name */
		attr = ldb_attr_casefold(ctx, el->name);
		if (attr == NULL) {
			return LDB_ERR_OPERATIONS_ERROR;
		}

		a = ldb_schema_attribute_by_name(ldb, el->name);

		switch (flags) {

		case LDB_FLAG_MOD_REPLACE:
			struct ldb_val *duplicate = NULL;

			ret = ldb_msg_find_duplicate_val(ldb, el, el,
							 &duplicate, 0);
			if (ret != LDB_SUCCESS) {
				return ret;
			}
			if (duplicate != NULL) {
				ldb_asprintf_errstring(
					ldb,
					"attribute '%s': value '%.*s' "
					"on '%s' provided more than "
					"once in REPLACE",
					el->name,
					(int)duplicate->length,
					duplicate->data,
					ldb_dn_get_linearized(msg2->dn));
				return LDB_ERR_ATTRIBUTE_OR_VALUE_EXISTS;
			}

			/* remove all attributes before adding the replacements */
			mod = lsqlite3_tprintf(ctx,
						"DELETE FROM ldb_attribute_values "
						"WHERE eid = '%lld' "
						"AND norm_attr_name = '%q';",
						eid, attr);
			if (mod == NULL) {
				return LDB_ERR_OPERATIONS_ERROR;
			}

			ret = sqlite3_exec(lsqlite3->sqlite, mod, NULL, NULL, &errmsg);
			if (ret != SQLITE_OK) {
				if (errmsg) {
					ldb_set_errstring(ldb, errmsg);
					free(errmsg);
				}
				return LDB_ERR_OPERATIONS_ERROR;
                        }

			/* MISSING break is INTENTIONAL */

		case LDB_FLAG_MOD_ADD:

			if (el->num_values == 0) {
				ldb_asprintf_errstring(ldb, "attribute %s on %s specified, but with 0 values (illigal)",
						       el->name, ldb_dn_get_linearized(msg->dn));
				return LDB_ERR_CONSTRAINT_VIOLATION;
			}

			/* For each value of the specified attribute name... */
			for (j = 0; j < el->num_values; j++) {
				struct ldb_val value;

				/* Get a canonicalised copy of the data */
				a->syntax->canonicalise_fn(ldb, ctx, &(el->values[j]), &value);
				if (value.data == NULL) {
					return LDB_ERR_OPERATIONS_ERROR;
				}

				mod = lsqlite3_tprintf(ctx,
					"INSERT OR ROLLBACK INTO ldb_attribute_values "
					"('eid', 'attr_name', 'norm_attr_name',"
					" 'attr_value', 'norm_attr_value') "
					"VALUES ('%lld', '%q', '%q', '%q', '%q');",
					eid, el->name, attr,
					el->values[j].data, value.data);

				if (mod == NULL) {
					return LDB_ERR_OPERATIONS_ERROR;
				}

				ret = sqlite3_exec(lsqlite3->sqlite, mod, NULL, NULL, &errmsg);
				if (ret != SQLITE_OK) {
					if (errmsg) {
						ldb_set_errstring(ldb, errmsg);
						free(errmsg);
					}
					return LDB_ERR_OPERATIONS_ERROR;
				}
			}

			break;

		case LDB_FLAG_MOD_DELETE:
#warning "We should throw an error if the attribute we are trying to delete does not exist!"
			if (el->num_values == 0) {
				mod = lsqlite3_tprintf(ctx,
							"DELETE FROM ldb_attribute_values "
							"WHERE eid = '%lld' "
							"AND norm_attr_name = '%q';",
							eid, attr);
				if (mod == NULL) {
					return LDB_ERR_OPERATIONS_ERROR;
				}

				ret = sqlite3_exec(lsqlite3->sqlite, mod, NULL, NULL, &errmsg);
				if (ret != SQLITE_OK) {
					if (errmsg) {
						ldb_set_errstring(ldb, errmsg);
						free(errmsg);
					}
					return LDB_ERR_OPERATIONS_ERROR;
                        	}
			}

			/* For each value of the specified attribute name... */
			for (j = 0; j < el->num_values; j++) {
				struct ldb_val value;

				/* Get a canonicalised copy of the data */
				a->syntax->canonicalise_fn(ldb, ctx, &(el->values[j]), &value);
				if (value.data == NULL) {
					return LDB_ERR_OPERATIONS_ERROR;
				}

				mod = lsqlite3_tprintf(ctx,
					"DELETE FROM ldb_attribute_values "
					"WHERE eid = '%lld' "
					"AND norm_attr_name = '%q' "
					"AND norm_attr_value = '%q';",
					eid, attr, value.data);

				if (mod == NULL) {
					return LDB_ERR_OPERATIONS_ERROR;
				}

				ret = sqlite3_exec(lsqlite3->sqlite, mod, NULL, NULL, &errmsg);
				if (ret != SQLITE_OK) {
					if (errmsg) {
						ldb_set_errstring(ldb, errmsg);
						free(errmsg);
					}
					return LDB_ERR_OPERATIONS_ERROR;
				}
			}

			break;
		}
	}

	return LDB_SUCCESS;
}