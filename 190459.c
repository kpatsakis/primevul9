static void test_wl4435()
{
  MYSQL_STMT *stmt;
  int        rc;
  char query[MAX_TEST_QUERY_LENGTH];

  char       str_data[20][WL4435_STRING_SIZE];
  double     dbl_data[20];
  char       dec_data[20][WL4435_STRING_SIZE];
  int        int_data[20];
  ulong      str_length= WL4435_STRING_SIZE;
  my_bool    is_null;
  MYSQL_BIND ps_params[WL4435_NUM_PARAMS];

  int exec_counter;

  myheader("test_wl4435");
  mct_start_logging("test_wl4435");

  rc= mysql_query(mysql, "DROP PROCEDURE IF EXISTS p1");
  myquery(rc);

  rc= mysql_query(mysql, "DROP PROCEDURE IF EXISTS p2");
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1");
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t2");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE t1(a1 INT, a2 CHAR(32), "
                       "  a3 DOUBLE(4, 2), a4 DECIMAL(3, 1))");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE t2(b0 INT, b1 INT, b2 CHAR(32), "
                       "  b3 DOUBLE(4, 2), b4 DECIMAL(3, 1))");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO t1 VALUES"
    "(1, '11', 12.34, 56.7), "
    "(2, '12', 56.78, 90.1), "
    "(3, '13', 23.45, 67.8)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO t2 VALUES"
    "(100, 10, '110', 70.70, 10.1), "
    "(200, 20, '120', 80.80, 20.2), "
    "(300, 30, '130', 90.90, 30.3)");
  myquery(rc);

  rc= mysql_query(mysql,
    "CREATE PROCEDURE p1("
    "   IN v0 INT, "
    "   OUT v_str_1 CHAR(32), "
    "   OUT v_dbl_1 DOUBLE(4, 2), "
    "   OUT v_dec_1 DECIMAL(6, 3), "
    "   OUT v_int_1 INT, "
    "   IN v1 INT, "
    "   INOUT v_str_2 CHAR(64), "
    "   INOUT v_dbl_2 DOUBLE(5, 3), "
    "   INOUT v_dec_2 DECIMAL(7, 4), "
    "   INOUT v_int_2 INT)"
    "BEGIN "
    "   SET v0 = -1; "
    "   SET v1 = -1; "
    "   SET v_str_1 = 'test_1'; "
    "   SET v_dbl_1 = 12.34; "
    "   SET v_dec_1 = 567.891; "
    "   SET v_int_1 = 2345; "
    "   SET v_str_2 = 'test_2'; "
    "   SET v_dbl_2 = 67.891; "
    "   SET v_dec_2 = 234.6789; "
    "   SET v_int_2 = 6789; "
    "   SELECT * FROM t1; "
    "   SELECT * FROM t2; "
    "END");
  myquery(rc);

  rc= mysql_query(mysql,
    "CREATE PROCEDURE p2("
    "   IN i1 VARCHAR(255) CHARACTER SET koi8r, "
    "   OUT o1 VARCHAR(255) CHARACTER SET cp1251, "
    "   OUT o2 VARBINARY(255)) "
    "BEGIN "
    "   SET o1 = i1; "
    "   SET o2 = i1; "
    "END");
  myquery(rc);

  strmov(query, "CALL p1(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  /* Init PS-parameters. */

  memset(ps_params, 0, sizeof (ps_params));

  /* - v0 -- INT */

  ps_params[0].buffer_type= MYSQL_TYPE_LONG;
  ps_params[0].buffer= (char *) &int_data[0];
  ps_params[0].length= 0;
  ps_params[0].is_null= 0;

  /* - v_str_1 -- CHAR(32) */

  ps_params[1].buffer_type= MYSQL_TYPE_STRING;
  ps_params[1].buffer= (char *) str_data[0];
  ps_params[1].buffer_length= WL4435_STRING_SIZE;
  ps_params[1].length= &str_length;
  ps_params[1].is_null= 0;

  /* - v_dbl_1 -- DOUBLE */

  ps_params[2].buffer_type= MYSQL_TYPE_DOUBLE;
  ps_params[2].buffer= (char *) &dbl_data[0];
  ps_params[2].length= 0;
  ps_params[2].is_null= 0;

  /* - v_dec_1 -- DECIMAL */

  ps_params[3].buffer_type= MYSQL_TYPE_NEWDECIMAL;
  ps_params[3].buffer= (char *) dec_data[0];
  ps_params[3].buffer_length= WL4435_STRING_SIZE;
  ps_params[3].length= 0;
  ps_params[3].is_null= 0;

  /* - v_int_1 -- INT */

  ps_params[4].buffer_type= MYSQL_TYPE_LONG;
  ps_params[4].buffer= (char *) &int_data[0];
  ps_params[4].length= 0;
  ps_params[4].is_null= 0;

  /* - v1 -- INT */

  ps_params[5].buffer_type= MYSQL_TYPE_LONG;
  ps_params[5].buffer= (char *) &int_data[0];
  ps_params[5].length= 0;
  ps_params[5].is_null= 0;

  /* - v_str_2 -- CHAR(32) */

  ps_params[6].buffer_type= MYSQL_TYPE_STRING;
  ps_params[6].buffer= (char *) str_data[0];
  ps_params[6].buffer_length= WL4435_STRING_SIZE;
  ps_params[6].length= &str_length;
  ps_params[6].is_null= 0;

  /* - v_dbl_2 -- DOUBLE */

  ps_params[7].buffer_type= MYSQL_TYPE_DOUBLE;
  ps_params[7].buffer= (char *) &dbl_data[0];
  ps_params[7].length= 0;
  ps_params[7].is_null= 0;

  /* - v_dec_2 -- DECIMAL */

  ps_params[8].buffer_type= MYSQL_TYPE_DECIMAL;
  ps_params[8].buffer= (char *) dec_data[0];
  ps_params[8].buffer_length= WL4435_STRING_SIZE;
  ps_params[8].length= 0;
  ps_params[8].is_null= 0;

  /* - v_int_2 -- INT */

  ps_params[9].buffer_type= MYSQL_TYPE_LONG;
  ps_params[9].buffer= (char *) &int_data[0];
  ps_params[9].length= 0;
  ps_params[9].is_null= 0;

  /* Bind parameters. */

  rc= mysql_stmt_bind_param(stmt, ps_params);

  /* Execute! */

  for (exec_counter= 0; exec_counter < 3; ++exec_counter)
  {
    int i;
    int num_fields;
    MYSQL_BIND *rs_bind;

    mct_log("\nexec_counter: %d\n", (int) exec_counter);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    while (1)
    {
      MYSQL_FIELD *fields;

      MYSQL_RES *rs_metadata= mysql_stmt_result_metadata(stmt);

      num_fields= mysql_stmt_field_count(stmt);
      fields= mysql_fetch_fields(rs_metadata);

      rs_bind= (MYSQL_BIND *) malloc(sizeof (MYSQL_BIND) * num_fields);
      memset(rs_bind, 0, sizeof (MYSQL_BIND) * num_fields);

      mct_log("num_fields: %d\n", (int) num_fields);

      for (i = 0; i < num_fields; ++i)
      {
        mct_log("  - %d: name: '%s'/'%s'; table: '%s'/'%s'; "
                "db: '%s'; catalog: '%s'; length: %d; max_length: %d; "
                "type: %d; decimals: %d\n",
                (int) i,
                (const char *) fields[i].name,
                (const char *) fields[i].org_name,
                (const char *) fields[i].table,
                (const char *) fields[i].org_table,
                (const char *) fields[i].db,
                (const char *) fields[i].catalog,
                (int) fields[i].length,
                (int) fields[i].max_length,
                (int) fields[i].type,
                (int) fields[i].decimals);

        rs_bind[i].buffer_type= fields[i].type;
        rs_bind[i].is_null= &is_null;

        switch (fields[i].type)
        {
          case MYSQL_TYPE_LONG:
            rs_bind[i].buffer= (char *) &(int_data[i]);
            rs_bind[i].buffer_length= sizeof (int_data);
            break;

          case MYSQL_TYPE_STRING:
            rs_bind[i].buffer= (char *) str_data[i];
            rs_bind[i].buffer_length= WL4435_STRING_SIZE;
            rs_bind[i].length= &str_length;
            break;

          case MYSQL_TYPE_DOUBLE:
            rs_bind[i].buffer= (char *) &dbl_data[i];
            rs_bind[i].buffer_length= sizeof (dbl_data);
            break;

          case MYSQL_TYPE_NEWDECIMAL:
            rs_bind[i].buffer= (char *) dec_data[i];
            rs_bind[i].buffer_length= WL4435_STRING_SIZE;
            rs_bind[i].length= &str_length;
            break;

          default:
            fprintf(stderr, "ERROR: unexpected type: %d.\n", fields[i].type);
            exit(1);
        }
      }

      rc= mysql_stmt_bind_result(stmt, rs_bind);
      check_execute(stmt, rc);

      mct_log("Data:\n");

      while (1)
      {
        int rc= mysql_stmt_fetch(stmt);

        if (rc == 1 || rc == MYSQL_NO_DATA)
          break;

        mct_log(" ");

        for (i = 0; i < num_fields; ++i)
        {
          switch (rs_bind[i].buffer_type)
          {
            case MYSQL_TYPE_LONG:
              mct_log(" int: %ld;",
                      (long) *((int *) rs_bind[i].buffer));
              break;

            case MYSQL_TYPE_STRING:
              mct_log(" str: '%s';",
                      (char *) rs_bind[i].buffer);
              break;

            case MYSQL_TYPE_DOUBLE:
              mct_log(" dbl: %lf;",
                      (double) *((double *) rs_bind[i].buffer));
              break;

            case MYSQL_TYPE_NEWDECIMAL:
              mct_log(" dec: '%s';",
                      (char *) rs_bind[i].buffer);
              break;

            default:
              printf("  unexpected type (%d)\n",
                rs_bind[i].buffer_type);
          }
        }
        mct_log("\n");
      }

      mct_log("EOF\n");

      rc= mysql_stmt_next_result(stmt);
      mct_log("mysql_stmt_next_result(): %d; field_count: %d\n",
              (int) rc, (int) mysql->field_count);

      free(rs_bind);
      mysql_free_result(rs_metadata);

      if (rc > 0)
      {
        printf("Error: %s (errno: %d)\n",
               mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
        DIE(rc > 0);
      }

      if (rc)
        break;

      if (!mysql->field_count)
      {
        /* This is the last OK-packet. No more resultsets. */
        break;
      }
    }

  }

  mysql_stmt_close(stmt);

  mct_close_log();

  rc= mysql_commit(mysql);
  myquery(rc);

  /* i18n part of test case. */

  {
    const char *str_koi8r= "\xee\xd5\x2c\x20\xda\xc1\x20\xd2\xd9\xc2\xc1\xcc\xcb\xd5";
    const char *str_cp1251= "\xcd\xf3\x2c\x20\xe7\xe0\x20\xf0\xfb\xe1\xe0\xeb\xea\xf3";
    char o1_buffer[255];
    ulong o1_length;
    char o2_buffer[255];
    ulong o2_length;

    MYSQL_BIND rs_bind[2];

    strmov(query, "CALL p2(?, ?, ?)");
    stmt= mysql_simple_prepare(mysql, query);
    check_stmt(stmt);

    /* Init PS-parameters. */

    memset(ps_params, 0, sizeof (ps_params));

    ps_params[0].buffer_type= MYSQL_TYPE_STRING;
    ps_params[0].buffer= (char *) str_koi8r;
    ps_params[0].buffer_length= strlen(str_koi8r);

    ps_params[1].buffer_type= MYSQL_TYPE_STRING;
    ps_params[1].buffer= o1_buffer;
    ps_params[1].buffer_length= 0;

    ps_params[2].buffer_type= MYSQL_TYPE_STRING;
    ps_params[2].buffer= o2_buffer;
    ps_params[2].buffer_length= 0;

    /* Bind parameters. */

    rc= mysql_stmt_bind_param(stmt, ps_params);
    check_execute(stmt, rc);

    /* Prevent converting to character_set_results. */

    rc= mysql_query(mysql, "SET NAMES binary");
    myquery(rc);

    /* Execute statement. */

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    /* Bind result. */

    memset(rs_bind, 0, sizeof (rs_bind));

    rs_bind[0].buffer_type= MYSQL_TYPE_STRING;
    rs_bind[0].buffer= o1_buffer;
    rs_bind[0].buffer_length= sizeof (o1_buffer);
    rs_bind[0].length= &o1_length;

    rs_bind[1].buffer_type= MYSQL_TYPE_BLOB;
    rs_bind[1].buffer= o2_buffer;
    rs_bind[1].buffer_length= sizeof (o2_buffer);
    rs_bind[1].length= &o2_length;

    rc= mysql_stmt_bind_result(stmt, rs_bind);
    check_execute(stmt, rc);

    /* Fetch result. */

    rc= mysql_stmt_fetch(stmt);
    check_execute(stmt, rc);

    /* Check result. */

    DIE_UNLESS(o1_length == strlen(str_cp1251));
    DIE_UNLESS(o2_length == strlen(str_koi8r));
    DIE_UNLESS(!memcmp(o1_buffer, str_cp1251, o1_length));
    DIE_UNLESS(!memcmp(o2_buffer, str_koi8r, o2_length));

    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc == MYSQL_NO_DATA);

    rc= mysql_stmt_next_result(stmt);
    DIE_UNLESS(rc == 0 && mysql->field_count == 0);

    mysql_stmt_close(stmt);

    rc= mysql_commit(mysql);
    myquery(rc);
  }
}