cmd_writekey (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  char *keyid;
  int force = has_option (line, "--force");
  unsigned char *keydata;
  size_t keydatalen;

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  line = skip_options (line);

  if (!*line)
    return set_error (GPG_ERR_ASS_PARAMETER, "no keyid given");
  keyid = line;
  while (*line && !spacep (line))
    line++;
  *line = 0;

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  if (!ctrl->app_ctx)
    return gpg_error (GPG_ERR_UNSUPPORTED_OPERATION);

  keyid = xtrystrdup (keyid);
  if (!keyid)
    return out_of_core ();

  /* Now get the actual keydata. */
  assuan_begin_confidential (ctx);
  rc = assuan_inquire (ctx, "KEYDATA", &keydata, &keydatalen, MAXLEN_KEYDATA);
  assuan_end_confidential (ctx);
  if (rc)
    {
      xfree (keyid);
      return rc;
    }

  /* Write the key to the card. */
  rc = app_writekey (ctrl->app_ctx, ctrl, keyid, force? 1:0,
                     pin_cb, ctx, keydata, keydatalen);
  xfree (keyid);
  xfree (keydata);

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}