cmd_random (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  size_t nbytes;
  unsigned char *buffer;

  if (!*line)
    return set_error (GPG_ERR_ASS_PARAMETER,
                      "number of requested bytes missing");
  nbytes = strtoul (line, NULL, 0);

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  if (!ctrl->app_ctx)
    return gpg_error (GPG_ERR_UNSUPPORTED_OPERATION);

  buffer = xtrymalloc (nbytes);
  if (!buffer)
    return out_of_core ();

  rc = app_get_challenge (ctrl->app_ctx, nbytes, buffer);
  if (!rc)
    {
      rc = assuan_send_data (ctx, buffer, nbytes);
      xfree (buffer);
      return rc; /* that is already an assuan error code */
    }
  xfree (buffer);

  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}