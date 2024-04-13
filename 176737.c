static int generate_binlog_opt_val(char **ret) {
  DBUG_ASSERT(ret);
  *ret = NULL;
  if (opt_bin_log && get_gtid_mode(GTID_MODE_LOCK_NONE) > GTID_MODE_OFF) {
    assert(opt_bin_logname);
    *ret = strcmp(opt_bin_logname, "0")
               ? my_strdup(key_memory_wsrep, opt_bin_logname, MYF(0))
               : my_strdup(key_memory_wsrep, "", MYF(0));
  } else {
    *ret = my_strdup(key_memory_wsrep, "", MYF(0));
  }
  if (!*ret) return -ENOMEM;
  return 0;
}