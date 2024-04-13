cmd_apdu (assuan_context_t ctx, char *line)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);
  int rc;
  unsigned char *apdu;
  size_t apdulen;
  int with_atr;
  int handle_more;
  const char *s;
  size_t exlen;
  int slot;

  if (has_option (line, "--dump-atr"))
    with_atr = 2;
  else
    with_atr = has_option (line, "--atr");
  handle_more = has_option (line, "--more");

  if ((s=has_option_name (line, "--exlen")))
    {
      if (*s == '=')
        exlen = strtoul (s+1, NULL, 0);
      else
        exlen = 4096;
    }
  else
    exlen = 0;

  line = skip_options (line);

  if ( IS_LOCKED (ctrl) )
    return gpg_error (GPG_ERR_LOCKED);

  if ((rc = open_card (ctrl, NULL)))
    return rc;

  slot = vreader_slot (ctrl->server_local->vreader_idx);

  if (with_atr)
    {
      unsigned char *atr;
      size_t atrlen;
      char hexbuf[400];

      atr = apdu_get_atr (slot, &atrlen);
      if (!atr || atrlen > sizeof hexbuf - 2 )
        {
          rc = gpg_error (GPG_ERR_INV_CARD);
          goto leave;
        }
      if (with_atr == 2)
        {
          char *string, *p, *pend;

          string = atr_dump (atr, atrlen);
          if (string)
            {
              for (rc=0, p=string; !rc && (pend = strchr (p, '\n')); p = pend+1)
                {
                  rc = assuan_send_data (ctx, p, pend - p + 1);
                  if (!rc)
                    rc = assuan_send_data (ctx, NULL, 0);
                }
              if (!rc && *p)
                rc = assuan_send_data (ctx, p, strlen (p));
              es_free (string);
              if (rc)
                goto leave;
            }
        }
      else
        {
          bin2hex (atr, atrlen, hexbuf);
          send_status_info (ctrl, "CARD-ATR", hexbuf, strlen (hexbuf), NULL, 0);
        }
      xfree (atr);
    }

  apdu = hex_to_buffer (line, &apdulen);
  if (!apdu)
    {
      rc = gpg_error_from_syserror ();
      goto leave;
    }
  if (apdulen)
    {
      unsigned char *result = NULL;
      size_t resultlen;

      rc = apdu_send_direct (slot, exlen,
                             apdu, apdulen, handle_more,
                             &result, &resultlen);
      if (rc)
        log_error ("apdu_send_direct failed: %s\n", gpg_strerror (rc));
      else
        {
          rc = assuan_send_data (ctx, result, resultlen);
          xfree (result);
        }
    }
  xfree (apdu);

 leave:
  TEST_CARD_REMOVAL (ctrl, rc);
  return rc;
}