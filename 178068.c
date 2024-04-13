void _firebird_error(pdo_dbh_t *dbh, pdo_stmt_t *stmt, char const *file, zend_long line) /* {{{ */
{
	pdo_error_type *const error_code = stmt ? &stmt->error_code : &dbh->error_code;

	strcpy(*error_code, "HY000");
}