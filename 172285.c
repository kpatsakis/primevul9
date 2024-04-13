query_int(const struct lsqlite3_private * lsqlite3,
          long long * pRet,
          const char * pSql,
          ...)
{
        int             ret;
        int             bLoop;
        char *          p;
        sqlite3_stmt *  pStmt;
        va_list         args;

        /* Begin access to variable argument list */
        va_start(args, pSql);

        /* Format the query */
        if ((p = sqlite3_vmprintf(pSql, args)) == NULL) {
		va_end(args);
                return SQLITE_NOMEM;
        }

        /*
         * Prepare and execute the SQL statement.  Loop allows retrying on
         * certain errors, e.g. SQLITE_SCHEMA occurs if the schema changes,
         * requiring retrying the operation.
         */
        for (bLoop = TRUE; bLoop; ) {

                /* Compile the SQL statement into sqlite virtual machine */
                if ((ret = sqlite3_prepare(lsqlite3->sqlite,
                                           p,
                                           -1,
                                           &pStmt,
                                           NULL)) == SQLITE_SCHEMA) {
                        if (stmtGetEID != NULL) {
                                sqlite3_finalize(stmtGetEID);
                                stmtGetEID = NULL;
                        }
                        continue;
                } else if (ret != SQLITE_OK) {
                        break;
                }

                /* One row expected */
                if ((ret = sqlite3_step(pStmt)) == SQLITE_SCHEMA) {
                        if (stmtGetEID != NULL) {
                                sqlite3_finalize(stmtGetEID);
                                stmtGetEID = NULL;
                        }
                        (void) sqlite3_finalize(pStmt);
                        continue;
                } else if (ret != SQLITE_ROW) {
                        (void) sqlite3_finalize(pStmt);
                        break;
                }

                /* Get the value to be returned */
                *pRet = sqlite3_column_int64(pStmt, 0);

                /* Free the virtual machine */
                if ((ret = sqlite3_finalize(pStmt)) == SQLITE_SCHEMA) {
                        if (stmtGetEID != NULL) {
                                sqlite3_finalize(stmtGetEID);
                                stmtGetEID = NULL;
                        }
                        continue;
                } else if (ret != SQLITE_OK) {
                        (void) sqlite3_finalize(pStmt);
                        break;
                }

                /*
                 * Normal condition is only one time through loop.  Loop is
                 * rerun in error conditions, via "continue", above.
                 */
                bLoop = FALSE;
        }

        /* All done with variable argument list */
        va_end(args);


        /* Free the memory we allocated for our query string */
        sqlite3_free(p);

        return ret;
}