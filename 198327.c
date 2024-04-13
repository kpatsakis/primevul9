static int valid_pgp_encrypted_handler (BODY *b, STATE *s)
{
  int rc;
  BODY *octetstream;

  octetstream = b->parts->next;

  /* clear out any mime headers before the handler, so they can't be
   * spoofed. */
  mutt_free_envelope (&b->mime_headers);
  mutt_free_envelope (&octetstream->mime_headers);

  /* Some clients improperly encode the octetstream part. */
  if (octetstream->encoding != ENC7BIT)
    rc = run_decode_and_handler (octetstream, s, crypt_pgp_encrypted_handler, 0);
  else
    rc = crypt_pgp_encrypted_handler (octetstream, s);
  b->goodsig |= octetstream->goodsig;
#ifdef USE_AUTOCRYPT
  b->is_autocrypt |= octetstream->is_autocrypt;
#endif

  /* Relocate protected headers onto the multipart/encrypted part */
  if (!rc && octetstream->mime_headers)
  {
    b->mime_headers = octetstream->mime_headers;
    octetstream->mime_headers = NULL;
  }

  return rc;
}