static int initialize(struct lsqlite3_private *lsqlite3,
		      struct ldb_context *ldb, const char *url,
		      unsigned int flags)
{
	TALLOC_CTX *local_ctx;
        long long queryInt;
	int rollback = 0;
	char *errmsg;
        char *schema;
        int ret;

	/* create a local ctx */
	local_ctx = talloc_named(lsqlite3, 0, "lsqlite3_rename local context");
	if (local_ctx == NULL) {
		return -1;
	}

	schema = lsqlite3_tprintf(local_ctx,


                "CREATE TABLE ldb_info AS "
                "  SELECT 'LDB' AS database_type,"
                "         '1.0' AS version;"

                /*
                 * The entry table holds the information about an entry.
                 * This table is used to obtain the EID of the entry and to
                 * support scope=one and scope=base.  The parent and child
                 * table is included in the entry table since all the other
                 * attributes are dependent on EID.
                 */
                "CREATE TABLE ldb_entry "
                "("
                "  eid     INTEGER PRIMARY KEY AUTOINCREMENT,"
                "  dn      TEXT UNIQUE NOT NULL,"
		"  norm_dn TEXT UNIQUE NOT NULL"
                ");"


                "CREATE TABLE ldb_object_classes"
                "("
                "  class_name            TEXT PRIMARY KEY,"
                "  parent_class_name     TEXT,"
                "  tree_key              TEXT UNIQUE,"
                "  max_child_num         INTEGER DEFAULT 0"
                ");"

                /*
                 * We keep a full listing of attribute/value pairs here
                 */
                "CREATE TABLE ldb_attribute_values"
                "("
                "  eid             INTEGER REFERENCES ldb_entry,"
                "  attr_name       TEXT,"
                "  norm_attr_name  TEXT,"
                "  attr_value      TEXT,"
                "  norm_attr_value TEXT "
                ");"


                /*
                 * Indexes
                 */
                "CREATE INDEX ldb_attribute_values_eid_idx "
                "  ON ldb_attribute_values (eid);"

                "CREATE INDEX ldb_attribute_values_name_value_idx "
                "  ON ldb_attribute_values (attr_name, norm_attr_value);"



                /*
                 * Triggers
                 */

                "CREATE TRIGGER ldb_object_classes_insert_tr"
                "  AFTER INSERT"
                "  ON ldb_object_classes"
                "  FOR EACH ROW"
                "    BEGIN"
                "      UPDATE ldb_object_classes"
                "        SET tree_key = COALESCE(tree_key, "
                "              ("
                "                SELECT tree_key || "
                "                       (SELECT base160(max_child_num + 1)"
                "                                FROM ldb_object_classes"
                "                                WHERE class_name = "
                "                                      new.parent_class_name)"
                "                  FROM ldb_object_classes "
                "                  WHERE class_name = new.parent_class_name "
                "              ));"
                "      UPDATE ldb_object_classes "
                "        SET max_child_num = max_child_num + 1"
                "        WHERE class_name = new.parent_class_name;"
                "    END;"

                /*
                 * Table initialization
                 */

                "INSERT INTO ldb_object_classes "
                "    (class_name, tree_key) "
                "  VALUES "
                "    ('TOP', '0001');");

        /* Skip protocol indicator of url  */
        if (strncmp(url, "sqlite3://", 10) != 0) {
                return SQLITE_MISUSE;
        }

        /* Update pointer to just after the protocol indicator */
        url += 10;

        /* Try to open the (possibly empty/non-existent) database */
        if ((ret = sqlite3_open(url, &lsqlite3->sqlite)) != SQLITE_OK) {
                return ret;
        }

        /* In case this is a new database, enable auto_vacuum */
	ret = sqlite3_exec(lsqlite3->sqlite, "PRAGMA auto_vacuum = 1;", NULL, NULL, &errmsg);
	if (ret != SQLITE_OK) {
		if (errmsg) {
			printf("lsqlite3 initializaion error: %s\n", errmsg);
			free(errmsg);
		}
		goto failed;
	}

	if (flags & LDB_FLG_NOSYNC) {
		/* DANGEROUS */
		ret = sqlite3_exec(lsqlite3->sqlite, "PRAGMA synchronous = OFF;", NULL, NULL, &errmsg);
		if (ret != SQLITE_OK) {
			if (errmsg) {
				printf("lsqlite3 initializaion error: %s\n", errmsg);
				free(errmsg);
			}
			goto failed;
		}
	}

	/* */

        /* Establish a busy timeout of 30 seconds */
        if ((ret = sqlite3_busy_timeout(lsqlite3->sqlite,
                                        30000)) != SQLITE_OK) {
                return ret;
        }

        /* Create a function, callable from sql, to increment a tree_key */
        if ((ret =
             sqlite3_create_function(lsqlite3->sqlite,/* handle */
                                     "base160_next",  /* function name */
                                     1,               /* number of args */
                                     SQLITE_ANY,      /* preferred text type */
                                     NULL,            /* user data */
                                     base160next_sql, /* called func */
                                     NULL,            /* step func */
                                     NULL             /* final func */
                     )) != SQLITE_OK) {
                return ret;
        }

        /* Create a function, callable from sql, to convert int to base160 */
        if ((ret =
             sqlite3_create_function(lsqlite3->sqlite,/* handle */
                                     "base160",       /* function name */
                                     1,               /* number of args */
                                     SQLITE_ANY,      /* preferred text type */
                                     NULL,            /* user data */
                                     base160_sql,     /* called func */
                                     NULL,            /* step func */
                                     NULL             /* final func */
                     )) != SQLITE_OK) {
                return ret;
        }

        /* Create a function, callable from sql, to perform various comparisons */
        if ((ret =
             sqlite3_create_function(lsqlite3->sqlite, /* handle */
                                     "ldap_compare",   /* function name */
                                     4,                /* number of args */
                                     SQLITE_ANY,       /* preferred text type */
                                     ldb  ,            /* user data */
                                     lsqlite3_compare, /* called func */
                                     NULL,             /* step func */
                                     NULL              /* final func */
                     )) != SQLITE_OK) {
                return ret;
        }

        /* Begin a transaction */
	ret = sqlite3_exec(lsqlite3->sqlite, "BEGIN EXCLUSIVE;", NULL, NULL, &errmsg);
	if (ret != SQLITE_OK) {
		if (errmsg) {
			printf("lsqlite3: initialization error: %s\n", errmsg);
			free(errmsg);
		}
		goto failed;
	}
	rollback = 1;

        /* Determine if this is a new database.  No tables means it is. */
        if (query_int(lsqlite3,
                      &queryInt,
                      "SELECT COUNT(*)\n"
                      "  FROM sqlite_master\n"
                      "  WHERE type = 'table';") != 0) {
		goto failed;
        }

        if (queryInt == 0) {
                /*
                 * Create the database schema
                 */
		ret = sqlite3_exec(lsqlite3->sqlite, schema, NULL, NULL, &errmsg);
		if (ret != SQLITE_OK) {
			if (errmsg) {
				printf("lsqlite3 initializaion error: %s\n", errmsg);
				free(errmsg);
			}
			goto failed;
		}
        } else {
                /*
                 * Ensure that the database we opened is one of ours
                 */
                if (query_int(lsqlite3,
                              &queryInt,
                              "SELECT "
                              "  (SELECT COUNT(*) = 2"
                              "     FROM sqlite_master "
                              "     WHERE type = 'table' "
                              "       AND name IN "
                              "         ("
                              "           'ldb_entry', "
                              "           'ldb_object_classes' "
                              "         ) "
                              "  ) "
                              "  AND "
                              "  (SELECT 1 "
                              "     FROM ldb_info "
                              "     WHERE database_type = 'LDB' "
                              "       AND version = '1.0'"
                              "  );") != 0 ||
                    queryInt != 1) {

                        /* It's not one that we created.  See ya! */
			goto failed;
                }
        }

        /* Commit the transaction */
	ret = sqlite3_exec(lsqlite3->sqlite, "COMMIT;", NULL, NULL, &errmsg);
	if (ret != SQLITE_OK) {
		if (errmsg) {
			printf("lsqlite3: iniialization error: %s\n", errmsg);
			free(errmsg);
		}
		goto failed;
	}

        return SQLITE_OK;

failed:
	if (rollback) lsqlite3_safe_rollback(lsqlite3->sqlite);
	sqlite3_close(lsqlite3->sqlite);
	return -1;
}