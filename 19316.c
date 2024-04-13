cmd_pkauth (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  unsigned char *outdata;
  size_t outdatalen;
  char *keyidstr;

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  if (!ctrl->app_ctx)
    return gpg_error (GPG_ERR_UNSUPPORTED_OPERATION);

 /* We have to use a copy of the key ID because the function may use
     the pin_cb which in turn uses the assuan line buffer and thus
     overwriting the original line with the keyid */
  keyidstr = xtrystrdup (line);
  if (!keyidstr)
    return out_of_core ();

  rc = app_auth (ctrl->app_ctx,
                 keyidstr,
                 pin_cb, ctx,
                 ctrl->in_data.value, ctrl->in_data.valuelen,
                 &outdata, &outdatalen);
  xfree (keyidstr);
  if (rc)
    {
      log_error ("app_auth failed: %s\n", gpg_strerror (rc));
    }
  else
    {
      rc = assuan_send_data (ctx, outdata, outdatalen);
      xfree (outdata);
      if (rc)
        return rc; /* that is already an assuan error code */
    }

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}