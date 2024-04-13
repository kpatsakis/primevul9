static void imap_set_changed_flag (CONTEXT *ctx, HEADER *h, int local_changes,
                                   int *server_changes, int flag_name, int old_hd_flag,
                                   int new_hd_flag, int h_flag)
{
  /* If there are local_changes, we only want to note if the server
   * flags have changed, so we can set a reopen flag in
   * cmd_parse_fetch().  We don't want to count a local modification
   * to the header flag as a "change".
   */
  if ((old_hd_flag != new_hd_flag) || (!local_changes))
  {
    if (new_hd_flag != h_flag)
    {
      if (server_changes)
        *server_changes = 1;

      /* Local changes have priority */
      if (!local_changes)
        mutt_set_flag (ctx, h, flag_name, new_hd_flag);
    }
  }
}