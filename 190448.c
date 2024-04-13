static void test_ps_query_cache()
{
  MYSQL      *lmysql= mysql;
  MYSQL_STMT *stmt;
  int        rc;
  MYSQL_BIND p_bind[2],r_bind[2]; /* p: param bind; r: result bind */
  int32      p_int_data, r_int_data;
  char       p_str_data[32], r_str_data[32];
  unsigned long p_str_length, r_str_length;
  MYSQL_RES  *r_metadata;
  char       query[MAX_TEST_QUERY_LENGTH];
  uint       hits1, hits2;
  enum enum_test_ps_query_cache
  {
    /*
      We iterate the same prepare/executes block, but have iterations where
      we vary the query cache conditions.
    */
    /* the query cache is enabled for the duration of prep&execs: */
    TEST_QCACHE_ON= 0,
    /*
      same but using a new connection (to see if qcache serves results from
      the previous connection as it should):
    */
    TEST_QCACHE_ON_WITH_OTHER_CONN,
    /*
      First border case: disables the query cache before prepare and
      re-enables it before execution (to test if we have no bug then):
    */
    TEST_QCACHE_OFF_ON,
    /*
      Second border case: enables the query cache before prepare and
      disables it before execution:
    */
    TEST_QCACHE_ON_OFF
  };
  enum enum_test_ps_query_cache iteration;

  myheader("test_ps_query_cache");

  rc= mysql_query(mysql, "SET SQL_MODE=''");
  myquery(rc);

  /* prepare the table */

  rc= mysql_query(mysql, "drop table if exists t1");
  myquery(rc);

  rc= mysql_query(mysql, "create table t1 (id1 int(11) NOT NULL default '0', "
                         "value2 varchar(100), value1 varchar(100))");
  myquery(rc);

  rc= mysql_query(mysql, "insert into t1 values (1, 'hh', 'hh'), "
                          "(2, 'hh', 'hh'), (1, 'ii', 'ii'), (2, 'ii', 'ii')");
  myquery(rc);

  for (iteration= TEST_QCACHE_ON; iteration <= TEST_QCACHE_ON_OFF; iteration++)
  {

    switch (iteration) {
    case TEST_QCACHE_ON:
    case TEST_QCACHE_ON_OFF:
      rc= mysql_query(lmysql, "set global query_cache_size=1000000");
      myquery(rc);
      break;
    case TEST_QCACHE_OFF_ON:
      rc= mysql_query(lmysql, "set global query_cache_size=0");
      myquery(rc);
      break;
    case TEST_QCACHE_ON_WITH_OTHER_CONN:
      if (!opt_silent)
        fprintf(stdout, "\n Establishing a test connection ...");
      if (!(lmysql= mysql_client_init(NULL)))
      {
        printf("mysql_client_init() failed");
        DIE_UNLESS(0);
      }
      if (!(mysql_real_connect(lmysql, opt_host, opt_user,
                               opt_password, current_db, opt_port,
                               opt_unix_socket, 0)))
      {
        printf("connection failed");
        mysql_close(lmysql);
        DIE_UNLESS(0);
      }
      rc= mysql_query(lmysql, "SET SQL_MODE=''");
      myquery(rc);

      if (!opt_silent)
        fprintf(stdout, "OK");
    }

    strmov(query, "select id1, value1 from t1 where id1= ? or "
           "CONVERT(value1 USING utf8)= ?");
    stmt= mysql_simple_prepare(lmysql, query);
    check_stmt(stmt);

    verify_param_count(stmt, 2);

    switch (iteration) {
    case TEST_QCACHE_OFF_ON:
      rc= mysql_query(lmysql, "set global query_cache_size=1000000");
      myquery(rc);
      break;
    case TEST_QCACHE_ON_OFF:
      rc= mysql_query(lmysql, "set global query_cache_size=0");
      myquery(rc);
    default:
      break;
    }

    memset(p_bind, 0, sizeof(p_bind));
    p_bind[0].buffer_type= MYSQL_TYPE_LONG;
    p_bind[0].buffer= (void *)&p_int_data;
    p_bind[1].buffer_type= MYSQL_TYPE_VAR_STRING;
    p_bind[1].buffer= (void *)p_str_data;
    p_bind[1].buffer_length= array_elements(p_str_data);
    p_bind[1].length= &p_str_length;

    rc= mysql_stmt_bind_param(stmt, p_bind);
    check_execute(stmt, rc);

    p_int_data= 1;
    strmov(p_str_data, "hh");
    p_str_length= strlen(p_str_data);

    memset(r_bind, 0, sizeof(r_bind));
    r_bind[0].buffer_type= MYSQL_TYPE_LONG;
    r_bind[0].buffer= (void *)&r_int_data;
    r_bind[1].buffer_type= MYSQL_TYPE_VAR_STRING;
    r_bind[1].buffer= (void *)r_str_data;
    r_bind[1].buffer_length= array_elements(r_str_data);
    r_bind[1].length= &r_str_length;

    rc= mysql_stmt_bind_result(stmt, r_bind);
    check_execute(stmt, rc);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    test_ps_query_cache_result(1, "hh", 2, 2, "hh", 2, 1, "ii", 2);

    /* now retry with the same parameter values and see qcache hits */
    hits1= query_cache_hits(lmysql);
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    test_ps_query_cache_result(1, "hh", 2, 2, "hh", 2, 1, "ii", 2);
    hits2= query_cache_hits(lmysql);
    switch(iteration) {
    case TEST_QCACHE_ON_WITH_OTHER_CONN:
    case TEST_QCACHE_ON:                 /* should have hit */
      DIE_UNLESS(hits2-hits1 == 1);
      break;
    case TEST_QCACHE_OFF_ON:
    case TEST_QCACHE_ON_OFF:             /* should not have hit */
      DIE_UNLESS(hits2-hits1 == 0);
      break;
    }

    /* now modify parameter values and see qcache hits */
    strmov(p_str_data, "ii");
    p_str_length= strlen(p_str_data);
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    test_ps_query_cache_result(1, "hh", 2, 1, "ii", 2, 2, "ii", 2);
    hits1= query_cache_hits(lmysql);

    switch(iteration) {
    case TEST_QCACHE_ON:
    case TEST_QCACHE_OFF_ON:
    case TEST_QCACHE_ON_OFF:             /* should not have hit */
      DIE_UNLESS(hits2-hits1 == 0);
      break;
    case TEST_QCACHE_ON_WITH_OTHER_CONN: /* should have hit */
      DIE_UNLESS(hits1-hits2 == 1);
      break;
    }

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    test_ps_query_cache_result(1, "hh", 2, 1, "ii", 2, 2, "ii", 2);
    hits2= query_cache_hits(lmysql);

    mysql_stmt_close(stmt);

    switch(iteration) {
    case TEST_QCACHE_ON:                 /* should have hit */
      DIE_UNLESS(hits2-hits1 == 1);
      break;
    case TEST_QCACHE_OFF_ON:
    case TEST_QCACHE_ON_OFF:             /* should not have hit */
      DIE_UNLESS(hits2-hits1 == 0);
      break;
    case TEST_QCACHE_ON_WITH_OTHER_CONN: /* should have hit */
      DIE_UNLESS(hits2-hits1 == 1);
      break;
    }

  } /* for(iteration=...) */

  if (lmysql != mysql)
    mysql_close(lmysql);

  rc= mysql_query(mysql, "set global query_cache_size=DEFAULT");
  myquery(rc);
}