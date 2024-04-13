static int firebird_handle_get_attribute(pdo_dbh_t *dbh, zend_long attr, zval *val) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	switch (attr) {
		char tmp[512];

		case PDO_ATTR_AUTOCOMMIT:
			ZVAL_LONG(val,dbh->auto_commit);
			return 1;

		case PDO_ATTR_CONNECTION_STATUS:
			ZVAL_BOOL(val, !isc_version(&H->db, firebird_info_cb, NULL));
			return 1;

		case PDO_ATTR_CLIENT_VERSION: {
#if defined(__GNUC__) || defined(PHP_WIN32)
			info_func_t info_func = NULL;
#ifdef __GNUC__
			info_func = (info_func_t)dlsym(RTLD_DEFAULT, "isc_get_client_version");
#else
			HMODULE l = GetModuleHandle("fbclient");

			if (!l) {
				break;
			}
			info_func = (info_func_t)GetProcAddress(l, "isc_get_client_version");
#endif
			if (info_func) {
				info_func(tmp);
				ZVAL_STRING(val, tmp);
			}
#else
			ZVAL_NULL(val);
#endif
			}
			return 1;

		case PDO_ATTR_SERVER_VERSION:
		case PDO_ATTR_SERVER_INFO:
			*tmp = 0;

			if (!isc_version(&H->db, firebird_info_cb, (void*)tmp)) {
				ZVAL_STRING(val, tmp);
				return 1;
			}

		case PDO_ATTR_FETCH_TABLE_NAMES:
			ZVAL_BOOL(val, H->fetch_table_names);
			return 1;
	}
	return 0;
}