static int firebird_handle_begin(pdo_dbh_t *dbh) /* {{{ */
{
	pdo_firebird_db_handle *H = (pdo_firebird_db_handle *)dbh->driver_data;
	char tpb[8] = { isc_tpb_version3 }, *ptpb = tpb+1;
#if abies_0
	if (dbh->transaction_flags & PDO_TRANS_ISOLATION_LEVEL) {
		if (dbh->transaction_flags & PDO_TRANS_READ_UNCOMMITTED) {
			/* this is a poor fit, but it's all we have */
			*ptpb++ = isc_tpb_read_committed;
			*ptpb++ = isc_tpb_rec_version;
			dbh->transaction_flags &= ~(PDO_TRANS_ISOLATION_LEVEL^PDO_TRANS_READ_UNCOMMITTED);
		} else if (dbh->transaction_flags & PDO_TRANS_READ_COMMITTED) {
			*ptpb++ = isc_tpb_read_committed;
			*ptpb++ = isc_tpb_no_rec_version;
			dbh->transaction_flags &= ~(PDO_TRANS_ISOLATION_LEVEL^PDO_TRANS_READ_COMMITTED);
		} else if (dbh->transaction_flags & PDO_TRANS_REPEATABLE_READ) {
			*ptpb++ = isc_tpb_concurrency;
			dbh->transaction_flags &= ~(PDO_TRANS_ISOLATION_LEVEL^PDO_TRANS_REPEATABLE_READ);
		} else {
			*ptpb++ = isc_tpb_consistency;
			dbh->transaction_flags &= ~(PDO_TRANS_ISOLATION_LEVEL^PDO_TRANS_SERIALIZABLE);
		}
	}

	if (dbh->transaction_flags & PDO_TRANS_ACCESS_MODE) {
		if (dbh->transaction_flags & PDO_TRANS_READONLY) {
			*ptpb++ = isc_tpb_read;
			dbh->transaction_flags &= ~(PDO_TRANS_ACCESS_MODE^PDO_TRANS_READONLY);
		} else {
			*ptpb++ = isc_tpb_write;
			dbh->transaction_flags &= ~(PDO_TRANS_ACCESS_MODE^PDO_TRANS_READWRITE);
		}
	}

	if (dbh->transaction_flags & PDO_TRANS_CONFLICT_RESOLUTION) {
		if (dbh->transaction_flags & PDO_TRANS_RETRY) {
			*ptpb++ = isc_tpb_wait;
			dbh->transaction_flags &= ~(PDO_TRANS_CONFLICT_RESOLUTION^PDO_TRANS_RETRY);
		} else {
			*ptpb++ = isc_tpb_nowait;
			dbh->transaction_flags &= ~(PDO_TRANS_CONFLICT_RESOLUTION^PDO_TRANS_ABORT);
		}
	}
#endif
	if (isc_start_transaction(H->isc_status, &H->tr, 1, &H->db, (unsigned short)(ptpb-tpb), tpb)) {
		RECORD_ERROR(dbh);
		return 0;
	}
	return 1;
}