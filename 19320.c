cmd_serialno (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc = 0;
  char *serial;
  time_t stamp;
  int retries = 0;

  /* Clear the remove flag so that the open_card is able to reread it.  */
 retry:
  if (ctrl->server_local->card_removed)
    {
      if ( IS_LOCKED (ctrl) )
        return gpg_error (GPG_ERR_LOCKED);
      do_reset (ctrl, 1);
    }

  if ((rc = open_card (ctrl, *line? line:NULL)))
    {
      /* In case of an inactive card, retry once.  */
      if (gpg_err_code (rc) == GPG_ERR_CARD_RESET && retries++ < 1)
        goto retry;
      return rc;
    }

  rc = app_get_serial_and_stamp (ctrl->app_ctx, &serial, &stamp);
  if (rc)
    return rc;

  rc = print_assuan_status (ctx, "SERIALNO", "%s %lu",
                            serial, (unsigned long)stamp);
  xfree (serial);
  return rc;
}