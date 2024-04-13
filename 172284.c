static int lsqlite3_search_callback(void *result, int col_num, char **cols, char **names)
{
	struct ldb_context *ldb;
	struct lsql_context *ac;
	struct ldb_message *msg;
	long long eid;
	unsigned int i;
	int ret;

	ac = talloc_get_type(result, struct lsql_context);
	ldb = ldb_module_get_ctx(ac->module);

	/* eid, dn, attr_name, attr_value */
	if (col_num != 4) return SQLITE_ABORT;

	eid = atoll(cols[0]);

	if (ac->ares) {
		msg = ac->ares->message;
	}

	if (eid != ac->current_eid) { /* here begin a new entry */

		/* call the async callback for the last entry
		 * except the first time */
		if (ac->current_eid != 0) {
			ret = ldb_msg_normalize(ldb, ac->req, msg, &msg);
			if (ret != LDB_SUCCESS) {
				return SQLITE_ABORT;
			}

			ret = ldb_module_send_entry(ac->req, msg, NULL);
			if (ret != LDB_SUCCESS) {
				ac->callback_failed = true;
				/* free msg object */
				TALLOC_FREE(msg);
				return SQLITE_ABORT;
			}

			/* free msg object */
			TALLOC_FREE(msg);
		}

		/* start over */
		ac->ares = talloc_zero(ac, struct ldb_reply);
		if (!ac->ares) return SQLITE_ABORT;

		msg = ldb_msg_new(ac->ares);
		if (!msg) return SQLITE_ABORT;

		ac->ares->type = LDB_REPLY_ENTRY;
		ac->current_eid = eid;
	}

	if (msg->dn == NULL) {
		msg->dn = ldb_dn_new(msg, ldb, cols[1]);
		if (msg->dn == NULL)
			return SQLITE_ABORT;
	}

	if (ac->attrs) {
		int found = 0;
		for (i = 0; ac->attrs[i]; i++) {
			if (strcasecmp(cols[2], ac->attrs[i]) == 0) {
				found = 1;
				break;
			}
		}
		if (!found) goto done;
	}

	if (ldb_msg_add_string(msg, cols[2], cols[3]) != 0) {
		return SQLITE_ABORT;
	}

done:
	ac->ares->message = msg;
	return SQLITE_OK;
}