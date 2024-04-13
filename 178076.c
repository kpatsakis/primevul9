static int firebird_alloc_prepare_stmt(pdo_dbh_t *dbh, const char *sql, size_t sql_len, /* {{{ */
	XSQLDA *out_sqlda, isc_stmt_handle *s, HashTable *named_params)
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;
	char *c, *new_sql, in_quote, in_param, pname[64], *ppname;
	zend_long l, pindex = -1;

	/* Firebird allows SQL statements up to 64k, so bail if it doesn't fit */
	if (sql_len > 65536) {
		strcpy(dbh->error_code, "01004");
		return 0;
	}

	/* start a new transaction implicitly if auto_commit is enabled and no transaction is open */
	if (dbh->auto_commit && !dbh->in_txn) {
		/* dbh->transaction_flags = PDO_TRANS_READ_UNCOMMITTED; */

		if (!firebird_handle_begin(dbh)) {
			return 0;
		}
		dbh->in_txn = 1;
	}

	/* allocate the statement */
	if (isc_dsql_allocate_statement(H->isc_status, &H->db, s)) {
		RECORD_ERROR(dbh);
		return 0;
	}

	/* in order to support named params, which Firebird itself doesn't,
	   we need to replace :foo by ?, and store the name we just replaced */
	new_sql = c = emalloc(sql_len+1);

	for (l = in_quote = in_param = 0; l <= sql_len; ++l) {
		if ( !(in_quote ^= (sql[l] == '\''))) {
			if (!in_param) {
				switch (sql[l]) {
					case ':':
						in_param = 1;
						ppname = pname;
						*ppname++ = sql[l];
					case '?':
						*c++ = '?';
						++pindex;
					continue;
				}
			} else {
                                if ((in_param &= ((sql[l] >= 'A' && sql[l] <= 'Z') || (sql[l] >= 'a' && sql[l] <= 'z')
                                        || (sql[l] >= '0' && sql[l] <= '9') || sql[l] == '_' || sql[l] == '-'))) {


					*ppname++ = sql[l];
					continue;
				} else {
					*ppname++ = 0;
					if (named_params) {
						zval tmp;
						ZVAL_LONG(&tmp, pindex);
						zend_hash_str_update(named_params, pname, (unsigned int)(ppname - pname - 1), &tmp);
					}
				}
			}
		}
		*c++ = sql[l];
	}

	/* prepare the statement */
	if (isc_dsql_prepare(H->isc_status, &H->tr, s, 0, new_sql, PDO_FB_DIALECT, out_sqlda)) {
		RECORD_ERROR(dbh);
		efree(new_sql);
		return 0;
	}

	efree(new_sql);
	return 1;
}