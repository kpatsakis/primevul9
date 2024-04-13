_pkcs1_rsa_verify_sig (const gnutls_datum_t * text,
		       const gnutls_datum_t * signature, bigint_t * params,
		       int params_len)
{
  gnutls_mac_algorithm_t hash = GNUTLS_MAC_UNKNOWN;
  int ret;
  opaque digest[MAX_HASH_SIZE], md[MAX_HASH_SIZE];
  int digest_size;
  digest_hd_st hd;
  gnutls_datum_t decrypted;

  ret =
    _gnutls_pkcs1_rsa_decrypt (&decrypted, signature, params, params_len, 1);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  /* decrypted is a BER encoded data of type DigestInfo
   */

  digest_size = sizeof (digest);
  if ((ret =
       decode_ber_digest_info (&decrypted, &hash, digest, &digest_size)) != 0)
    {
      gnutls_assert ();
      _gnutls_free_datum (&decrypted);
      return ret;
    }

  _gnutls_free_datum (&decrypted);

  if (digest_size != _gnutls_hash_get_algo_len (hash))
    {
      gnutls_assert ();
      return GNUTLS_E_ASN1_GENERIC_ERROR;
    }

  ret = _gnutls_hash_init (&hd, hash);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  _gnutls_hash (&hd, text->data, text->size);
  _gnutls_hash_deinit (&hd, md);

  if (memcmp (md, digest, digest_size) != 0)
    {
      gnutls_assert ();
      return GNUTLS_E_PK_SIG_VERIFY_FAILED;
    }

  return 0;
}