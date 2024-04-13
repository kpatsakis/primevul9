int mutt_body_handler (BODY *b, STATE *s)
{
  int plaintext = 0;
  handler_t handler = NULL, encrypted_handler = NULL;
  int rc = 0;
  static unsigned short recurse_level = 0;

  int oflags = s->flags;

  if (recurse_level >= MUTT_MIME_MAX_DEPTH)
  {
    dprint (1, (debugfile, "mutt_body_handler: recurse level too deep. giving up!\n"));
    return 1;
  }
  recurse_level++;

  /* first determine which handler to use to process this part */

  if (mutt_is_autoview (b))
  {
    handler = autoview_handler;
    s->flags &= ~MUTT_CHARCONV;
  }
  else if (b->type == TYPETEXT)
  {
    if (ascii_strcasecmp ("plain", b->subtype) == 0)
    {
      /* avoid copying this part twice since removing the transfer-encoding is
       * the only operation needed.
       */
      if ((WithCrypto & APPLICATION_PGP) && mutt_is_application_pgp (b))
	encrypted_handler = handler = crypt_pgp_application_pgp_handler;
      else if (option(OPTREFLOWTEXT) && ascii_strcasecmp ("flowed", mutt_get_parameter ("format", b->parameter)) == 0)
	handler = rfc3676_handler;
      else
	handler = text_plain_handler;
    }
    else if (ascii_strcasecmp ("enriched", b->subtype) == 0)
      handler = text_enriched_handler;
    else /* text body type without a handler */
      plaintext = 1;
  }
  else if (b->type == TYPEMESSAGE)
  {
    if (mutt_is_message_type(b->type, b->subtype))
      handler = message_handler;
    else if (!ascii_strcasecmp ("delivery-status", b->subtype))
      plaintext = 1;
    else if (!ascii_strcasecmp ("external-body", b->subtype))
      handler = external_body_handler;
  }
  else if (b->type == TYPEMULTIPART)
  {
    char *p;

    if (ascii_strcasecmp ("alternative", b->subtype) == 0)
      handler = alternative_handler;
    else if (WithCrypto && ascii_strcasecmp ("signed", b->subtype) == 0)
    {
      p = mutt_get_parameter ("protocol", b->parameter);

      if (!p)
        mutt_error _("Error: multipart/signed has no protocol.");
      else if (s->flags & MUTT_VERIFY)
	handler = mutt_signed_handler;
    }
    else if (mutt_is_valid_multipart_pgp_encrypted (b))
      encrypted_handler = handler = valid_pgp_encrypted_handler;
    else if (mutt_is_malformed_multipart_pgp_encrypted (b))
      encrypted_handler = handler = malformed_pgp_encrypted_handler;

    if (!handler)
      handler = multipart_handler;

    if (b->encoding != ENC7BIT && b->encoding != ENC8BIT
        && b->encoding != ENCBINARY)
    {
      dprint (1, (debugfile, "Bad encoding type %d for multipart entity, "
                  "assuming 7 bit\n", b->encoding));
      b->encoding = ENC7BIT;
    }
  }
  else if (WithCrypto && b->type == TYPEAPPLICATION)
  {
    if (option (OPTDONTHANDLEPGPKEYS)
        && !ascii_strcasecmp("pgp-keys", b->subtype))
    {
      /* pass raw part through for key extraction */
      plaintext = 1;
    }
    else if ((WithCrypto & APPLICATION_PGP) && mutt_is_application_pgp (b))
      encrypted_handler = handler = crypt_pgp_application_pgp_handler;
    else if ((WithCrypto & APPLICATION_SMIME) && mutt_is_application_smime(b))
      encrypted_handler = handler = crypt_smime_application_smime_handler;
  }

  /* only respect disposition == attachment if we're not
     displaying from the attachment menu (i.e. pager) */
  if ((!option (OPTHONORDISP) || (b->disposition != DISPATTACH ||
				  option(OPTVIEWATTACH))) &&
      (plaintext || handler))
  {
    /* Prevent encrypted attachments from being included in replies
     * unless $include_encrypted is set. */
    if ((s->flags & MUTT_REPLYING) &&
        (s->flags & MUTT_FIRSTDONE) &&
        encrypted_handler &&
        !option (OPTINCLUDEENCRYPTED))
      goto cleanup;

    rc = run_decode_and_handler (b, s, handler, plaintext);
  }
  /* print hint to use attachment menu for disposition == attachment
     if we're not already being called from there */
  else if (s->flags & MUTT_DISPLAY)
  {
    state_mark_attach (s);
    if (option (OPTHONORDISP) && b->disposition == DISPATTACH)
      fputs (_("[-- This is an attachment "), s->fpout);
    else
      state_printf (s, _("[-- %s/%s is unsupported "), TYPE (b), b->subtype);
    if (!option (OPTVIEWATTACH))
    {
      char keystroke[SHORT_STRING];

      if (km_expand_key (keystroke, sizeof(keystroke),
                         km_find_func (MENU_PAGER, OP_VIEW_ATTACHMENTS)))
	fprintf (s->fpout, _("(use '%s' to view this part)"), keystroke);
      else
	fputs (_("(need 'view-attachments' bound to key!)"), s->fpout);
    }
    fputs (" --]\n", s->fpout);
  }

cleanup:
  recurse_level--;
  s->flags = oflags | (s->flags & MUTT_FIRSTDONE);
  if (rc)
  {
    dprint (1, (debugfile, "Bailing on attachment of type %s/%s.\n", TYPE(b), NONULL (b->subtype)));
  }

  return rc;
}