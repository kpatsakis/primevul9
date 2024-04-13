cmd_checkpin (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  char *idstr;

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  if (!ctrl->app_ctx)
    return gpg_error (GPG_ERR_UNSUPPORTED_OPERATION);

  /* We have to use a copy of the key ID because the function may use
     the pin_cb which in turn uses the assuan line buffer and thus
     overwriting the original line with the keyid. */
  idstr = xtrystrdup (line);
  if (!idstr)
    return out_of_core ();

  rc = app_check_pin (ctrl->app_ctx, idstr, pin_cb, ctx);
  xfree (idstr);
  if (rc)
    log_error ("app_check_pin failed: %s\n", gpg_strerror (rc));

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}