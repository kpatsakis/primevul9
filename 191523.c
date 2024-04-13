static void ldb_trace_request(struct ldb_context *ldb, struct ldb_request *req)
{
	TALLOC_CTX *tmp_ctx = talloc_new(req);
	unsigned int i;
	struct ldb_ldif ldif;

	switch (req->operation) {
	case LDB_SEARCH:
		ldb_debug_add(ldb, "ldb_trace_request: SEARCH\n");
		ldb_debug_add(ldb, " dn: %s\n",
			      ldb_dn_is_null(req->op.search.base)?"<rootDSE>":
			      ldb_dn_get_linearized(req->op.search.base));
		ldb_debug_add(ldb, " scope: %s\n", 
			  req->op.search.scope==LDB_SCOPE_BASE?"base":
			  req->op.search.scope==LDB_SCOPE_ONELEVEL?"one":
			  req->op.search.scope==LDB_SCOPE_SUBTREE?"sub":"UNKNOWN");
		ldb_debug_add(ldb, " expr: %s\n", 
			  ldb_filter_from_tree(tmp_ctx, req->op.search.tree));
		if (req->op.search.attrs == NULL) {
			ldb_debug_add(ldb, " attr: <ALL>\n");
		} else {
			for (i=0; req->op.search.attrs[i]; i++) {
				ldb_debug_add(ldb, " attr: %s\n", req->op.search.attrs[i]);
			}
		}
		break;
	case LDB_DELETE:
		ldb_debug_add(ldb, "ldb_trace_request: DELETE\n");
		ldb_debug_add(ldb, " dn: %s\n", 
			      ldb_dn_get_linearized(req->op.del.dn));
		break;
	case LDB_RENAME:
		ldb_debug_add(ldb, "ldb_trace_request: RENAME\n");
		ldb_debug_add(ldb, " olddn: %s\n", 
			      ldb_dn_get_linearized(req->op.rename.olddn));
		ldb_debug_add(ldb, " newdn: %s\n", 
			      ldb_dn_get_linearized(req->op.rename.newdn));
		break;
	case LDB_EXTENDED:
		ldb_debug_add(ldb, "ldb_trace_request: EXTENDED\n");
		ldb_debug_add(ldb, " oid: %s\n", req->op.extended.oid);
		ldb_debug_add(ldb, " data: %s\n", req->op.extended.data?"yes":"no");
		break;
	case LDB_ADD:
		ldif.changetype = LDB_CHANGETYPE_ADD;
		ldif.msg = discard_const_p(struct ldb_message, req->op.add.message);

		ldb_debug_add(ldb, "ldb_trace_request: ADD\n");

		/* 
		 * The choice to call
		 * ldb_ldif_write_redacted_trace_string() is CRITICAL
		 * for security.  It ensures that we do not output
		 * passwords into debug logs 
		 */

		ldb_debug_add(req->handle->ldb, "%s\n", 
			      ldb_ldif_write_redacted_trace_string(req->handle->ldb, tmp_ctx, &ldif));
		break;
	case LDB_MODIFY:
		ldif.changetype = LDB_CHANGETYPE_MODIFY;
		ldif.msg = discard_const_p(struct ldb_message, req->op.mod.message);

		ldb_debug_add(ldb, "ldb_trace_request: MODIFY\n");

		/* 
		 * The choice to call
		 * ldb_ldif_write_redacted_trace_string() is CRITICAL
		 * for security.  It ensures that we do not output
		 * passwords into debug logs 
		 */

		ldb_debug_add(req->handle->ldb, "%s\n", 
			      ldb_ldif_write_redacted_trace_string(req->handle->ldb, tmp_ctx, &ldif));
		break;
	case LDB_REQ_REGISTER_CONTROL:
		ldb_debug_add(ldb, "ldb_trace_request: REGISTER_CONTROL\n");
		ldb_debug_add(req->handle->ldb, "%s\n", 
			      req->op.reg_control.oid);
		break;
	case LDB_REQ_REGISTER_PARTITION:
		ldb_debug_add(ldb, "ldb_trace_request: REGISTER_PARTITION\n");
		ldb_debug_add(req->handle->ldb, "%s\n", 
			      ldb_dn_get_linearized(req->op.reg_partition.dn));
		break;
	default:
		ldb_debug_add(ldb, "ldb_trace_request: UNKNOWN(%u)\n", 
			      req->operation);
		break;
	}

	if (req->controls == NULL) {
		ldb_debug_add(ldb, " control: <NONE>\n");
	} else {
		for (i=0; req->controls && req->controls[i]; i++) {
			if (req->controls[i]->oid) {
				ldb_debug_add(ldb, " control: %s  crit:%u  data:%s\n",
					      req->controls[i]->oid,
					      req->controls[i]->critical,
					      req->controls[i]->data?"yes":"no");
			}
		}
	}
	
	ldb_debug_end(ldb, LDB_DEBUG_TRACE);

	talloc_free(tmp_ctx);
}