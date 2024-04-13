static _Bool check_receive_okay(const value_list_t *vl) /* {{{ */
{
  uint64_t time_sent = 0;
  int status;

  status = uc_meta_data_get_unsigned_int(vl, "network:time_sent", &time_sent);

  /* This is a value we already sent. Don't allow it to be received again in
   * order to avoid looping. */
  if ((status == 0) && (time_sent >= ((uint64_t)vl->time)))
    return (0);

  return (1);
} /* }}} _Bool check_receive_okay */