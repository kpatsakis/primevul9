static int pdo_firebird_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;
	const ISC_STATUS *s = H->isc_status;
	char buf[400];
	zend_long i = 0, l, sqlcode = isc_sqlcode(s);

	if (sqlcode) {
		add_next_index_long(info, sqlcode);

		while ((sizeof(buf)>(i+2))&&(l = fb_interpret(&buf[i],(sizeof(buf)-i-2),&s))) {
			i += l;
			strcpy(&buf[i++], " ");
		}
		add_next_index_string(info, buf);
	} else if (H->last_app_error) {
		add_next_index_long(info, -999);
		add_next_index_string(info, const_cast(H->last_app_error));
	}
	return 1;
}