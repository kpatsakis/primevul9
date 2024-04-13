static void test_temporal_param()
{
#define N_PARAMS 3
  MYSQL_STMT   *stmt= 0;
  uint         rc;
  ulong        length[N_PARAMS],  length2[N_PARAMS];
  MYSQL_BIND   my_bind[N_PARAMS], my_bind2[N_PARAMS];
  my_bool      is_null[N_PARAMS], is_null2[N_PARAMS];
  MYSQL_TIME   tm;
  longlong     bigint= 123;
  double       real= 123;
  char         dec[40];

  /* Initialize param/fetch buffers for data, null flags, lengths */
  memset(&my_bind, 0, sizeof(my_bind));
  memset(&my_bind2, 0, sizeof(my_bind2));
  memset(&length, 0, sizeof(length));
  memset(&length2, 0, sizeof(length2));
  memset(&is_null, 0, sizeof(is_null));
  memset(&is_null2, 0, sizeof(is_null2));

  /* Initialize the first input parameter */
  my_bind[0].buffer_type= MYSQL_TYPE_TIMESTAMP;
  my_bind[0].buffer= (void *) &tm;
  my_bind[0].is_null= &is_null[0];
  my_bind[0].length= &length[0];
  my_bind[0].buffer_length= sizeof(tm);

  /* Clone the second and the third input parameter */
  my_bind[2]= my_bind[1]= my_bind[0];

  /* Initialize fetch parameters */
  my_bind2[0].buffer_type= MYSQL_TYPE_LONGLONG;
  my_bind2[0].length= &length2[0];
  my_bind2[0].is_null= &is_null2[0];
  my_bind2[0].buffer_length= sizeof(bigint);
  my_bind2[0].buffer= (void *) &bigint;

  my_bind2[1].buffer_type= MYSQL_TYPE_DOUBLE;
  my_bind2[1].length= &length2[1];
  my_bind2[1].is_null= &is_null2[1];
  my_bind2[1].buffer_length= sizeof(real);
  my_bind2[1].buffer= (void *) &real;

  my_bind2[2].buffer_type= MYSQL_TYPE_STRING;
  my_bind2[2].length= &length2[2];
  my_bind2[2].is_null= &is_null2[2];
  my_bind2[2].buffer_length= sizeof(dec);
  my_bind2[2].buffer= (void *) &dec;


  /* Prepare and bind input and output parameters */
  stmt= mysql_simple_prepare(mysql, "SELECT CAST(? AS SIGNED), ?+0e0, ?+0.0");
  check_stmt(stmt);
  verify_param_count(stmt, N_PARAMS);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind2);
  check_execute(stmt, rc);

  /* Initialize DATETIME value */
  tm.neg= 0;
  tm.time_type= MYSQL_TIMESTAMP_DATETIME;
  tm.year= 2001;
  tm.month= 10;
  tm.day= 20;
  tm.hour= 10;
  tm.minute= 10;
  tm.second= 59;
  tm.second_part= 500000;

  /* Execute and fetch */
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    printf("\n%lld %f '%s'\n", bigint, real, dec);

  /* Check values.  */
  DIE_UNLESS(bigint ==  20011020101100LL);
  DIE_UNLESS(real == 20011020101059.5);
  DIE_UNLESS(!strcmp(dec, "20011020101059.5"));

  mysql_stmt_close(stmt);

  /* Re-initialize input parameters to TIME data type */
  my_bind[0].buffer_type= my_bind[1].buffer_type=
                          my_bind[2].buffer_type= MYSQL_TYPE_TIME;

  /* Prepare and bind intput and output parameters */
  stmt= mysql_simple_prepare(mysql, "SELECT CAST(? AS SIGNED), ?+0e0, ?+0.0");
  check_stmt(stmt);
  verify_param_count(stmt, N_PARAMS);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind2);
  check_execute(stmt, rc);

  /* Initialize TIME value */
  tm.neg= 0;
  tm.time_type= MYSQL_TIMESTAMP_TIME;
  tm.year= tm.month= tm.day= 0;
  tm.hour= 10;
  tm.minute= 10;
  tm.second= 59;
  tm.second_part= 500000;

  /* Execute and fetch */
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    printf("\n%lld %f '%s'\n", bigint, real, dec);

  /* Check returned values */
  DIE_UNLESS(bigint ==  101100);
  DIE_UNLESS(real ==  101059.5);
  DIE_UNLESS(!strcmp(dec, "101059.5"));

  mysql_stmt_close(stmt);
}