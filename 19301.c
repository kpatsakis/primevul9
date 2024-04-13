cmd_passwd (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  char *chvnostr;
  unsigned int flags = 0;

  if (has_option (line, "--reset"))
    flags |= APP_CHANGE_FLAG_RESET;
  if (has_option (line, "--nullpin"))
    flags |= APP_CHANGE_FLAG_NULLPIN;

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  line = skip_options (line);

  if (!*line)
    return set_error (GPG_ERR_ASS_PARAMETER, "no CHV number given");
  chvnostr = line;
  while (*line && !spacep (line))
    line++;
  *line = 0;

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  if (!ctrl->app_ctx)
    return gpg_error (GPG_ERR_UNSUPPORTED_OPERATION);

  chvnostr = xtrystrdup (chvnostr);
  if (!chvnostr)
    return out_of_core ();
  rc = app_change_pin (ctrl->app_ctx, ctrl, chvnostr, flags, pin_cb, ctx);
  if (rc)
    log_error ("command passwd failed: %s\n", gpg_strerror (rc));
  xfree (chvnostr);

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}