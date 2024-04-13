cmd_writecert (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  char *certid;
  unsigned char *certdata;
  size_t certdatalen;

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  line = skip_options (line);

  if (!*line)
    return set_error (GPG_ERR_ASS_PARAMETER, "no certid given");
  certid = line;
  while (*line && !spacep (line))
    line++;
  *line = 0;

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  if (!ctrl->app_ctx)
    return gpg_error (GPG_ERR_UNSUPPORTED_OPERATION);

  certid = xtrystrdup (certid);
  if (!certid)
    return out_of_core ();

  /* Now get the actual keydata. */
  rc = assuan_inquire (ctx, "CERTDATA",
                       &certdata, &certdatalen, MAXLEN_CERTDATA);
  if (rc)
    {
      xfree (certid);
      return rc;
    }

  /* Write the certificate to the card. */
  rc = app_writecert (ctrl->app_ctx, ctrl, certid,
                      pin_cb, ctx, certdata, certdatalen);
  xfree (certid);
  xfree (certdata);

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}