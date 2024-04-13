pin_cb (void *opaque, const char *info, char **retstr)
{
  assuan_context_t ctx = opaque;
  char *command;
  int rc;
  unsigned char *value;
  size_t valuelen;

  if (!retstr)
    {
      /* We prompt for pinpad entry.  To make sure that the popup has
         been show we use an inquire and not just a status message.
         We ignore any value returned.  */
      if (info)
        {
          log_debug ("prompting for pinpad entry '%s'\n", info);
          rc = gpgrt_asprintf (&command, "POPUPPINPADPROMPT %s", info);
          if (rc < 0)
            return gpg_error (gpg_err_code_from_errno (errno));
          rc = assuan_inquire (ctx, command, &value, &valuelen, MAXLEN_PIN);
          xfree (command);
        }
      else
        {
          log_debug ("dismiss pinpad entry prompt\n");
          rc = assuan_inquire (ctx, "DISMISSPINPADPROMPT",
                               &value, &valuelen, MAXLEN_PIN);
        }
      if (!rc)
        xfree (value);
      return rc;
    }

  *retstr = NULL;
  log_debug ("asking for PIN '%s'\n", info);

  rc = gpgrt_asprintf (&command, "NEEDPIN %s", info);
  if (rc < 0)
    return gpg_error (gpg_err_code_from_errno (errno));

  /* Fixme: Write an inquire function which returns the result in
     secure memory and check all further handling of the PIN. */
  rc = assuan_inquire (ctx, command, &value, &valuelen, MAXLEN_PIN);
  xfree (command);
  if (rc)
    return rc;

  if (!valuelen || value[valuelen-1])
    {
      /* We require that the returned value is an UTF-8 string */
      xfree (value);
      return gpg_error (GPG_ERR_INV_RESPONSE);
    }
  *retstr = (char*)value;
  return 0;
}