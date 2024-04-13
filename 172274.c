int lsql_search(struct lsql_context *ctx)
{
	struct ldb_module *module = ctx->module;
	struct ldb_request *req = ctx->req;
	struct lsqlite3_private *lsqlite3;
	struct ldb_context *ldb;
	char *norm_basedn;
	char *sqlfilter;
	char *errmsg;
	char *query = NULL;
        int ret;

	ldb = ldb_module_get_ctx(module);
	lsqlite3 = talloc_get_type(ldb_module_get_private(module),
				   struct lsqlite3_private);

	if ((( ! ldb_dn_is_valid(req->op.search.base)) ||
	     ldb_dn_is_null(req->op.search.base)) &&
	    (req->op.search.scope == LDB_SCOPE_BASE ||
	     req->op.search.scope == LDB_SCOPE_ONELEVEL)) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	if (req->op.search.base) {
		norm_basedn = ldb_dn_alloc_casefold(ctx, req->op.search.base);
		if (norm_basedn == NULL) {
			return LDB_ERR_OPERATIONS_ERROR;
		}
	} else norm_basedn = talloc_strdup(ctx, "");

        /* Convert filter into a series of SQL conditions (constraints) */
	sqlfilter = parsetree_to_sql(module, ctx, req->op.search.tree);

        switch(req->op.search.scope) {
        case LDB_SCOPE_DEFAULT:
        case LDB_SCOPE_SUBTREE:
		if (*norm_basedn != '\0') {
			query = lsqlite3_tprintf(ctx,
				"SELECT entry.eid,\n"
				"       entry.dn,\n"
				"       av.attr_name,\n"
				"       av.attr_value\n"
				"  FROM ldb_entry AS entry\n"

				"  LEFT OUTER JOIN ldb_attribute_values AS av\n"
				"    ON av.eid = entry.eid\n"

				"  WHERE entry.eid IN\n"
				"    (SELECT DISTINCT ldb_entry.eid\n"
				"       FROM ldb_entry\n"
				"       WHERE (ldb_entry.norm_dn GLOB('*,%q')\n"
				"       OR ldb_entry.norm_dn = '%q')\n"
				"       AND ldb_entry.eid IN\n"
				"         (%s)\n"
				"    )\n"

				"  ORDER BY entry.eid ASC;",
				norm_basedn,
				norm_basedn,
				sqlfilter);
		} else {
			query = lsqlite3_tprintf(ctx,
				"SELECT entry.eid,\n"
				"       entry.dn,\n"
				"       av.attr_name,\n"
				"       av.attr_value\n"
				"  FROM ldb_entry AS entry\n"

				"  LEFT OUTER JOIN ldb_attribute_values AS av\n"
				"    ON av.eid = entry.eid\n"

				"  WHERE entry.eid IN\n"
				"    (SELECT DISTINCT ldb_entry.eid\n"
				"       FROM ldb_entry\n"
				"       WHERE ldb_entry.eid IN\n"
				"         (%s)\n"
				"    )\n"

				"  ORDER BY entry.eid ASC;",
				sqlfilter);
		}

		break;

        case LDB_SCOPE_BASE:
                query = lsqlite3_tprintf(ctx,
                        "SELECT entry.eid,\n"
                        "       entry.dn,\n"
                        "       av.attr_name,\n"
                        "       av.attr_value\n"
                        "  FROM ldb_entry AS entry\n"

                        "  LEFT OUTER JOIN ldb_attribute_values AS av\n"
                        "    ON av.eid = entry.eid\n"

                        "  WHERE entry.eid IN\n"
                        "    (SELECT DISTINCT ldb_entry.eid\n"
                        "       FROM ldb_entry\n"
                        "       WHERE ldb_entry.norm_dn = '%q'\n"
                        "         AND ldb_entry.eid IN\n"
			"           (%s)\n"
                        "    )\n"

                        "  ORDER BY entry.eid ASC;",
			norm_basedn,
                        sqlfilter);
                break;

        case LDB_SCOPE_ONELEVEL:
                query = lsqlite3_tprintf(ctx,
                        "SELECT entry.eid,\n"
                        "       entry.dn,\n"
                        "       av.attr_name,\n"
                        "       av.attr_value\n"
                        "  FROM ldb_entry AS entry\n"

                        "  LEFT OUTER JOIN ldb_attribute_values AS av\n"
                        "    ON av.eid = entry.eid\n"

                        "  WHERE entry.eid IN\n"
                        "    (SELECT DISTINCT ldb_entry.eid\n"
                        "       FROM ldb_entry\n"
			"       WHERE norm_dn GLOB('*,%q')\n"
			"         AND NOT norm_dn GLOB('*,*,%q')\n"
                        "         AND ldb_entry.eid IN\n(%s)\n"
                        "    )\n"

                        "  ORDER BY entry.eid ASC;",
                        norm_basedn,
                        norm_basedn,
                        sqlfilter);
                break;
        }

        if (query == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
        }

	/* * /
	printf ("%s\n", query);
	/ * */

	ctx->current_eid = 0;
	ctx->attrs = req->op.search.attrs;
	ctx->ares = NULL;

	ldb_request_set_state(req, LDB_ASYNC_PENDING);

	ret = sqlite3_exec(lsqlite3->sqlite, query, lsqlite3_search_callback, ctx, &errmsg);
	if (ret != SQLITE_OK) {
		if (errmsg) {
			ldb_set_errstring(ldb, errmsg);
			free(errmsg);
		}
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* complete the last message if any */
	if (ctx->ares) {
		ret = ldb_msg_normalize(ldb, ctx->ares,
		                        ctx->ares->message,
		                        &ctx->ares->message);
		if (ret != LDB_SUCCESS) {
			return LDB_ERR_OPERATIONS_ERROR;
		}

		ret = ldb_module_send_entry(req, ctx->ares->message, NULL);
		if (ret != LDB_SUCCESS) {
			return ret;
		}
	}


	return LDB_SUCCESS;
}