int ldb_request(struct ldb_context *ldb, struct ldb_request *req)
{
	struct ldb_module *next_module;
	int ret;

	if (req->callback == NULL) {
		ldb_set_errstring(ldb, "Requests MUST define callbacks");
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	ldb_reset_err_string(ldb);

	if (ldb->flags & LDB_FLG_ENABLE_TRACING) {
		ldb_trace_request(ldb, req);
	}

	/* call the first module in the chain */
	switch (req->operation) {
	case LDB_SEARCH:
	{
		/*
		 * A fake module to allow ldb_next_request() to be
		 * re-used and to keep the locking out of this function.
		 */
		static const struct ldb_module_ops lock_module_ops = {
			.name = "lock_searches",
			.search = lock_search
		};
		struct ldb_module lock_module = {
			.ldb = ldb,
			.next = ldb->modules,
			.ops = &lock_module_ops
		};
		next_module = &lock_module;

		/* due to "ldb_build_search_req" base DN always != NULL */
		if (!ldb_dn_validate(req->op.search.base)) {
			ldb_asprintf_errstring(ldb, "ldb_search: invalid basedn '%s'",
					       ldb_dn_get_linearized(req->op.search.base));
			return LDB_ERR_INVALID_DN_SYNTAX;
		}

		ret = next_module->ops->search(next_module, req);
		break;
	}
	case LDB_ADD:
		if (!ldb_dn_validate(req->op.add.message->dn)) {
			ldb_asprintf_errstring(ldb, "ldb_add: invalid dn '%s'",
					       ldb_dn_get_linearized(req->op.add.message->dn));
			return LDB_ERR_INVALID_DN_SYNTAX;
		}
		/*
		 * we have to normalize here, as so many places
		 * in modules and backends assume we don't have two
		 * elements with the same name
		 */
		ret = ldb_msg_normalize(ldb, req, req->op.add.message,
		                        discard_const(&req->op.add.message));
		if (ret != LDB_SUCCESS) {
			ldb_oom(ldb);
			return ret;
		}
		FIRST_OP(ldb, add);
		ret = ldb_msg_check_element_flags(ldb, req->op.add.message);
		if (ret != LDB_SUCCESS) {
			/*
			 * "ldb_msg_check_element_flags" generates an error
			 * string
			 */
			return ret;
		}
		ret = next_module->ops->add(next_module, req);
		break;
	case LDB_MODIFY:
		if (!ldb_dn_validate(req->op.mod.message->dn)) {
			ldb_asprintf_errstring(ldb, "ldb_modify: invalid dn '%s'",
					       ldb_dn_get_linearized(req->op.mod.message->dn));
			return LDB_ERR_INVALID_DN_SYNTAX;
		}
		FIRST_OP(ldb, modify);
		ret = ldb_msg_check_element_flags(ldb, req->op.mod.message);
		if (ret != LDB_SUCCESS) {
			/*
			 * "ldb_msg_check_element_flags" generates an error
			 * string
			 */
			return ret;
		}
		ret = next_module->ops->modify(next_module, req);
		break;
	case LDB_DELETE:
		if (!ldb_dn_validate(req->op.del.dn)) {
			ldb_asprintf_errstring(ldb, "ldb_delete: invalid dn '%s'",
					       ldb_dn_get_linearized(req->op.del.dn));
			return LDB_ERR_INVALID_DN_SYNTAX;
		}
		FIRST_OP(ldb, del);
		ret = next_module->ops->del(next_module, req);
		break;
	case LDB_RENAME:
		if (!ldb_dn_validate(req->op.rename.olddn)) {
			ldb_asprintf_errstring(ldb, "ldb_rename: invalid olddn '%s'",
					       ldb_dn_get_linearized(req->op.rename.olddn));
			return LDB_ERR_INVALID_DN_SYNTAX;
		}
		if (!ldb_dn_validate(req->op.rename.newdn)) {
			ldb_asprintf_errstring(ldb, "ldb_rename: invalid newdn '%s'",
					       ldb_dn_get_linearized(req->op.rename.newdn));
			return LDB_ERR_INVALID_DN_SYNTAX;
		}
		FIRST_OP(ldb, rename);
		ret = next_module->ops->rename(next_module, req);
		break;
	case LDB_EXTENDED:
		FIRST_OP(ldb, extended);
		ret = next_module->ops->extended(next_module, req);
		break;
	default:
		FIRST_OP(ldb, request);
		ret = next_module->ops->request(next_module, req);
		break;
	}

	if ((ret != LDB_SUCCESS) && (ldb->err_string == NULL)) {
		/* if no error string was setup by the backend */
		ldb_asprintf_errstring(ldb, "ldb_request: %s (%d)",
				       ldb_strerror(ret), ret);
	}

	return ret;
}