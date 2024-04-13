static int firebird_handle_rollback(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;

	if (isc_rollback_transaction(H->isc_status, &H->tr)) {
		RECORD_ERROR(dbh);
		return 0;
	}
	return 1;
}