cmd_readcert (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  unsigned char *cert;
  size_t ncert;

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  line = xstrdup (line); /* Need a copy of the line. */
  rc = app_readcert (ctrl->app_ctx, line, &cert, &ncert);
  if (rc)
    log_error ("app_readcert failed: %s\n", gpg_strerror (rc));
  xfree (line);
  line = NULL;
  if (!rc)
    {
      rc = assuan_send_data (ctx, cert, ncert);
      xfree (cert);
      if (rc)
        return rc;
    }

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}