void mutt_decode_attachment (const BODY *b, STATE *s)
{
  int istext = mutt_is_text_part (b);
  iconv_t cd = (iconv_t)(-1);

  if (istext && s->flags & MUTT_CHARCONV)
  {
    char *charset = mutt_get_parameter ("charset", b->parameter);
    if (!charset && AssumedCharset)
      charset = mutt_get_default_charset ();
    if (charset && Charset)
      cd = mutt_iconv_open (Charset, charset, MUTT_ICONV_HOOK_FROM);
  }
  else if (istext && b->charset)
    cd = mutt_iconv_open (Charset, b->charset, MUTT_ICONV_HOOK_FROM);

  fseeko (s->fpin, b->offset, SEEK_SET);
  switch (b->encoding)
  {
    case ENCQUOTEDPRINTABLE:
      mutt_decode_quoted (s, b->length, istext || ((WithCrypto & APPLICATION_PGP) && mutt_is_application_pgp (b)), cd);
      break;
    case ENCBASE64:
      mutt_decode_base64 (s, b->length, istext || ((WithCrypto & APPLICATION_PGP) && mutt_is_application_pgp (b)), cd);
      break;
    case ENCUUENCODED:
      mutt_decode_uuencoded (s, b->length, istext || ((WithCrypto & APPLICATION_PGP) && mutt_is_application_pgp (b)), cd);
      break;
    default:
      mutt_decode_xbit (s, b->length, istext || ((WithCrypto & APPLICATION_PGP) && mutt_is_application_pgp (b)), cd);
      break;
  }

  if (cd != (iconv_t)(-1))
    iconv_close (cd);
}