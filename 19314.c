cmd_genkey (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  char *keyno;
  int force;
  const char *s;
  time_t timestamp;

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  force = has_option (line, "--force");

  if ((s=has_option_name (line, "--timestamp")))
    {
      if (*s != '=')
        return set_error (GPG_ERR_ASS_PARAMETER, "missing value for option");
      timestamp = isotime2epoch (s+1);
      if (timestamp < 1)
        return set_error (GPG_ERR_ASS_PARAMETER, "invalid time value");
    }
  else
    timestamp = 0;


  line = skip_options (line);
  if (!*line)
    return set_error (GPG_ERR_ASS_PARAMETER, "no key number given");
  keyno = line;
  while (*line && !spacep (line))
    line++;
  *line = 0;

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  if (!ctrl->app_ctx)
    return gpg_error (GPG_ERR_UNSUPPORTED_OPERATION);

  keyno = xtrystrdup (keyno);
  if (!keyno)
    return out_of_core ();
  rc = app_genkey (ctrl->app_ctx, ctrl, keyno, force? 1:0,
                   timestamp, pin_cb, ctx);
  xfree (keyno);

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}