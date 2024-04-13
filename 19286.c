cmd_unlock (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc = 0;

  (void)line;

  if (locked_session)
    {
      if (locked_session != ctrl->server_local)
        rc = gpg_error (GPG_ERR_LOCKED);
      else
        locked_session = NULL;
    }
  else
    rc = gpg_error (GPG_ERR_NOT_LOCKED);

  if (rc)
    log_error ("cmd_unlock failed: %s\n", gpg_strerror (rc));
  return rc;
}