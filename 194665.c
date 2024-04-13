receive_check_fs(int msg_size)
{
int space, inodes;

if (check_spool_space > 0 || msg_size > 0 || check_spool_inodes > 0)
  {
  space = receive_statvfs(TRUE, &inodes);

  DEBUG(D_receive)
    debug_printf("spool directory space = %dK inodes = %d "
      "check_space = %dK inodes = %d msg_size = %d\n",
      space, inodes, check_spool_space, check_spool_inodes, msg_size);

  if ((space >= 0 && space < check_spool_space) ||
      (inodes >= 0 && inodes < check_spool_inodes))
    {
    log_write(0, LOG_MAIN, "spool directory space check failed: space=%d "
      "inodes=%d", space, inodes);
    return FALSE;
    }
  }

if (check_log_space > 0 || check_log_inodes > 0)
  {
  space = receive_statvfs(FALSE, &inodes);

  DEBUG(D_receive)
    debug_printf("log directory space = %dK inodes = %d "
      "check_space = %dK inodes = %d\n",
      space, inodes, check_log_space, check_log_inodes);

  if ((space >= 0 && space < check_log_space) ||
      (inodes >= 0 && inodes < check_log_inodes))
    {
    log_write(0, LOG_MAIN, "log directory space check failed: space=%d "
      "inodes=%d", space, inodes);
    return FALSE;
    }
  }

return TRUE;
}