static int firebird_handle_set_attribute(pdo_dbh_t *dbh, zend_long attr, zval *val) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	switch (attr) {
		case PDO_ATTR_AUTOCOMMIT:
			{
				zend_bool bval = zval_get_long(val)? 1 : 0;

				/* ignore if the new value equals the old one */
				if (dbh->auto_commit ^ bval) {
					if (dbh->in_txn) {
						if (bval) {
							/* turning on auto_commit with an open transaction is illegal, because
							   we won't know what to do with it */
							H->last_app_error = "Cannot enable auto-commit while a transaction is already open";
							return 0;
						} else {
							/* close the transaction */
							if (!firebird_handle_commit(dbh)) {
								break;
							}
							dbh->in_txn = 0;
						}
					}
					dbh->auto_commit = bval;
				}
			}
			return 1;

		case PDO_ATTR_FETCH_TABLE_NAMES:
			H->fetch_table_names = zval_get_long(val)? 1 : 0;
			return 1;

		case PDO_FB_ATTR_DATE_FORMAT:
			{
				zend_string *str = zval_get_string(val);
				if (H->date_format) {
					efree(H->date_format);
				}
				spprintf(&H->date_format, 0, "%s", ZSTR_VAL(str));
				zend_string_release_ex(str, 0);
			}
			return 1;

		case PDO_FB_ATTR_TIME_FORMAT:
			{
				zend_string *str = zval_get_string(val);
				if (H->time_format) {
					efree(H->time_format);
				}
				spprintf(&H->time_format, 0, "%s", ZSTR_VAL(str));
				zend_string_release_ex(str, 0);
			}
			return 1;

		case PDO_FB_ATTR_TIMESTAMP_FORMAT:
			{
				zend_string *str = zval_get_string(val);
				if (H->timestamp_format) {
					efree(H->timestamp_format);
				}
				spprintf(&H->timestamp_format, 0, "%s", ZSTR_VAL(str));
				zend_string_release_ex(str, 0);
			}
			return 1;
	}
	return 0;
}