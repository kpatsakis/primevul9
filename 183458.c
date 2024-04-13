_gnutls_server_find_pk_algos_in_ciphersuites (const opaque *
					      data, int datalen)
{
  int j;
  gnutls_pk_algorithm_t algo = GNUTLS_PK_NONE, prev_algo = 0;
  gnutls_kx_algorithm_t kx;
  cipher_suite_st cs;

  if (datalen % 2 != 0)
    {
      gnutls_assert ();
      return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
    }

  for (j = 0; j < datalen; j += 2)
    {
      memcpy (&cs.suite, &data[j], 2);
      kx = _gnutls_cipher_suite_get_kx_algo (&cs);

      if (_gnutls_map_kx_get_cred (kx, 1) == GNUTLS_CRD_CERTIFICATE)
	{
	  algo = _gnutls_map_pk_get_pk (kx);

	  if (algo != prev_algo && prev_algo != 0)
	    return GNUTLS_PK_ANY;
	  prev_algo = algo;
	}
    }

  return algo;
}