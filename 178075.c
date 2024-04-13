static int firebird_handle_closer(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	if (dbh->in_txn) {
		if (dbh->auto_commit) {
			if (isc_commit_transaction(H->isc_status, &H->tr)) {
				RECORD_ERROR(dbh);
			}
		} else {
			if (isc_rollback_transaction(H->isc_status, &H->tr)) {
				RECORD_ERROR(dbh);
			}
		}
	}

	if (isc_detach_database(H->isc_status, &H->db)) {
		RECORD_ERROR(dbh);
	}

	if (H->date_format) {
		efree(H->date_format);
	}
	if (H->time_format) {
		efree(H->time_format);
	}
	if (H->timestamp_format) {
		efree(H->timestamp_format);
	}

	pefree(H, dbh->is_persistent);

	return 0;
}