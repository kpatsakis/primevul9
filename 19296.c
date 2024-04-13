cmd_pkdecrypt (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  unsigned char *outdata;
  size_t outdatalen;
  char *keyidstr;
  unsigned int infoflags;

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  keyidstr = xtrystrdup (line);
  if (!keyidstr)
    return out_of_core ();
  rc = app_decipher (ctrl->app_ctx,
                     keyidstr,
                     pin_cb, ctx,
                     ctrl->in_data.value, ctrl->in_data.valuelen,
                     &outdata, &outdatalen, &infoflags);

  xfree (keyidstr);
  if (rc)
    {
      log_error ("app_decipher failed: %s\n", gpg_strerror (rc));
    }
  else
    {
      /* If the card driver told us that there is no padding, send a
         status line.  If there is a padding it is assumed that the
         caller knows what padding is used.  It would have been better
         to always send that information but for backward
         compatibility we can't do that.  */
      if ((infoflags & APP_DECIPHER_INFO_NOPAD))
        send_status_direct (ctrl, "PADDING", "0");
      rc = assuan_send_data (ctx, outdata, outdatalen);
      xfree (outdata);
      if (rc)
        return rc; /* that is already an assuan error code */
    }

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}