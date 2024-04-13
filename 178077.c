static int pdo_firebird_handle_factory(pdo_dbh_t *dbh, zval *driver_options) /* {{{ */
{
	struct pdo_data_src_parser vars[] = {
		{ "dbname", NULL, 0 },
		{ "charset",  NULL,	0 },
		{ "role", NULL,	0 }
	};
	int i, ret = 0;
	short buf_len = 256, dpb_len;

	pdo_firebird_db_handle *H = dbh->driver_data = pecalloc(1,sizeof(*H),dbh->is_persistent);

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 3);

	do {
		static char const dpb_flags[] = {
			isc_dpb_user_name, isc_dpb_password, isc_dpb_lc_ctype, isc_dpb_sql_role_name };
		char const *dpb_values[] = { dbh->username, dbh->password, vars[1].optval, vars[2].optval };
		char dpb_buffer[256] = { isc_dpb_version1 }, *dpb;

		dpb = dpb_buffer + 1;

		/* loop through all the provided arguments and set dpb fields accordingly */
		for (i = 0; i < sizeof(dpb_flags); ++i) {
			if (dpb_values[i] && buf_len > 0) {
				dpb_len = slprintf(dpb, buf_len, "%c%c%s", dpb_flags[i], (unsigned char)strlen(dpb_values[i]),
					dpb_values[i]);
				dpb += dpb_len;
				buf_len -= dpb_len;
			}
		}

		/* fire it up baby! */
		if (isc_attach_database(H->isc_status, 0, vars[0].optval, &H->db,(short)(dpb-dpb_buffer), dpb_buffer)) {
			break;
		}

		dbh->methods = &firebird_methods;
		dbh->native_case = PDO_CASE_UPPER;
		dbh->alloc_own_columns = 1;

		ret = 1;

	} while (0);

	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); ++i) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	if (!dbh->methods) {
		char errmsg[512];
		const ISC_STATUS *s = H->isc_status;
		fb_interpret(errmsg, sizeof(errmsg),&s);
		zend_throw_exception_ex(php_pdo_get_exception(), H->isc_status[1], "SQLSTATE[%s] [%d] %s",
				"HY000", H->isc_status[1], errmsg);
	}

	if (!ret) {
		firebird_handle_closer(dbh);
	}

	return ret;
}