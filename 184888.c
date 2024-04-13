check_if_same_cert (gnutls_x509_crt_t cert1, gnutls_x509_crt_t cert2)
{
  gnutls_datum_t cert1bin = { NULL, 0 }, cert2bin = { NULL, 0 };
  int result;

  result = _gnutls_x509_der_encode (cert1->cert, "", &cert1bin, 0);
  if (result < 0)
    {
      gnutls_assert ();
      goto cleanup;
    }

  result = _gnutls_x509_der_encode (cert2->cert, "", &cert2bin, 0);
  if (result < 0)
    {
      gnutls_assert ();
      goto cleanup;
    }

  if ((cert1bin.size == cert2bin.size) &&
      (memcmp (cert1bin.data, cert2bin.data, cert1bin.size) == 0))
    result = 0;
  else
    result = 1;

 cleanup:
  _gnutls_free_datum (&cert1bin);
  _gnutls_free_datum (&cert2bin);
  return result;
}