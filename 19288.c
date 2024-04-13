cmd_lock (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc = 0;

 retry:
  if (locked_session)
    {
      if (locked_session != ctrl->server_local)
        rc = gpg_error (GPG_ERR_LOCKED);
    }
  else
    locked_session = ctrl->server_local;

#ifdef USE_NPTH
  if (rc && has_option (line, "--wait"))
    {
      rc = 0;
      npth_sleep (1); /* Better implement an event mechanism. However,
			 for card operations this should be
			 sufficient. */
      /* FIXME: Need to check that the connection is still alive.
         This can be done by issuing status messages. */
      goto retry;
    }
#endif /*USE_NPTH*/

  if (rc)
    log_error ("cmd_lock failed: %s\n", gpg_strerror (rc));
  return rc;
}