static void test_wl5924()
{
  int rc;
  MYSQL *l_mysql;
  MYSQL_RES *res;
  MYSQL_ROW row;

  myheader("test_wl5924");
  l_mysql= mysql_client_init(NULL);
  DIE_UNLESS(l_mysql != NULL);

  /* we want a non-default character set */
  rc= mysql_set_character_set(l_mysql, "cp1251");
  DIE_UNLESS(rc == 0);

  /* put in an attr */
  rc= mysql_options4(l_mysql, MYSQL_OPT_CONNECT_ATTR_ADD,
                     "key1", "value1");
  DIE_UNLESS(rc == 0);

  /* put a second attr */
  rc= mysql_options4(l_mysql, MYSQL_OPT_CONNECT_ATTR_ADD,
                     "key2", "value2");
  DIE_UNLESS(rc == 0);

  /* put the second attr again : should fail */
  rc= mysql_options4(l_mysql, MYSQL_OPT_CONNECT_ATTR_ADD,
                     "key2", "value2");
  DIE_UNLESS(rc != 0);

  /* delete the second attr */
  rc= mysql_options(l_mysql, MYSQL_OPT_CONNECT_ATTR_DELETE,
                    "key2");
  DIE_UNLESS(rc == 0);

  /* put the second attr again : should pass */
  rc= mysql_options4(l_mysql, MYSQL_OPT_CONNECT_ATTR_ADD,
                     "key2", "value2");
  DIE_UNLESS(rc == 0);

  /* full reset */
  rc= mysql_options(l_mysql, MYSQL_OPT_CONNECT_ATTR_RESET, NULL);
  DIE_UNLESS(rc == 0);

  /* put the second attr again : should pass */
  rc= mysql_options4(l_mysql, MYSQL_OPT_CONNECT_ATTR_ADD,
                     "key2", "value2");
  DIE_UNLESS(rc == 0);

  /* full reset */
  rc= mysql_options(l_mysql, MYSQL_OPT_CONNECT_ATTR_RESET, NULL);
  DIE_UNLESS(rc == 0);

  /* add a third attr */
  rc= mysql_options4(l_mysql, MYSQL_OPT_CONNECT_ATTR_ADD,
                     "key3", "value3");
  DIE_UNLESS(rc == 0);

  /* add a fourth attr */
  rc= mysql_options4(l_mysql, MYSQL_OPT_CONNECT_ATTR_ADD,
                     "key4", "value4");
  DIE_UNLESS(rc == 0);

  /* add a non-ascii attr */
  /* note : this is Георги, Кодинов in windows-1251 */
  rc= mysql_options4(l_mysql, MYSQL_OPT_CONNECT_ATTR_ADD,
                     "\xc3\xe5\xee\xf0\xe3\xe8",
                     "\xca\xee\xe4\xe8\xed\xee\xe2");
  DIE_UNLESS(rc == 0);

  l_mysql= mysql_real_connect(l_mysql, opt_host, opt_user,
                         opt_password, current_db, opt_port,
                         opt_unix_socket, 0);
  DIE_UNLESS(l_mysql != 0);

  rc= mysql_query(l_mysql,
                  "SELECT ATTR_NAME, ATTR_VALUE "
                  " FROM performance_schema.session_account_connect_attrs"
                  " WHERE ATTR_NAME IN ('key1','key2','key3','key4',"
                  "  '\xc3\xe5\xee\xf0\xe3\xe8') AND"
                  "  PROCESSLIST_ID = CONNECTION_ID() ORDER BY ATTR_NAME");
  myquery2(l_mysql,rc);
  res= mysql_use_result(l_mysql);
  DIE_UNLESS(res);

  row= mysql_fetch_row(res);
  DIE_UNLESS(row);
  DIE_UNLESS(0 == strcmp(row[0], "key3"));
  DIE_UNLESS(0 == strcmp(row[1], "value3"));

  row= mysql_fetch_row(res);
  DIE_UNLESS(row);
  DIE_UNLESS(0 == strcmp(row[0], "key4"));
  DIE_UNLESS(0 == strcmp(row[1], "value4"));

  row= mysql_fetch_row(res);
  DIE_UNLESS(row);
  DIE_UNLESS(0 == strcmp(row[0], "\xc3\xe5\xee\xf0\xe3\xe8"));
  DIE_UNLESS(0 == strcmp(row[1], "\xca\xee\xe4\xe8\xed\xee\xe2"));

  mysql_free_result(res);

  l_mysql->reconnect= 1;
  rc= mysql_reconnect(l_mysql);
  myquery2(l_mysql,rc);

  mysql_close(l_mysql);
}