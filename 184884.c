dsa_verify_sig (const gnutls_datum_t * text,
		const gnutls_datum_t * signature, bigint_t * params,
		int params_len)
{
  int ret;
  opaque _digest[MAX_HASH_SIZE];
  gnutls_datum_t digest;
  digest_hd_st hd;

  ret = _gnutls_hash_init (&hd, GNUTLS_MAC_SHA1);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  _gnutls_hash (&hd, text->data, text->size);
  _gnutls_hash_deinit (&hd, _digest);

  digest.data = _digest;
  digest.size = 20;

  ret = _gnutls_dsa_verify (&digest, signature, params, params_len);

  return ret;
}