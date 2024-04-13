static gcry_cipher_hd_t network_get_aes256_cypher (sockent_t *se, /* {{{ */
    const void *iv, size_t iv_size, const char *username)
{
  gcry_error_t err;
  gcry_cipher_hd_t *cyper_ptr;
  unsigned char password_hash[32];

  if (se->type == SOCKENT_TYPE_CLIENT)
  {
	  cyper_ptr = &se->data.client.cypher;
	  memcpy (password_hash, se->data.client.password_hash,
			  sizeof (password_hash));
  }
  else
  {
	  char *secret;

	  cyper_ptr = &se->data.server.cypher;

	  if (username == NULL)
		  return (NULL);

	  secret = fbh_get (se->data.server.userdb, username);
	  if (secret == NULL)
		  return (NULL);

	  gcry_md_hash_buffer (GCRY_MD_SHA256,
			  password_hash,
			  secret, strlen (secret));

	  sfree (secret);
  }

  if (*cyper_ptr == NULL)
  {
    err = gcry_cipher_open (cyper_ptr,
        GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_OFB, /* flags = */ 0);
    if (err != 0)
    {
      ERROR ("network plugin: gcry_cipher_open returned: %s",
          gcry_strerror (err));
      *cyper_ptr = NULL;
      return (NULL);
    }
  }
  else
  {
    gcry_cipher_reset (*cyper_ptr);
  }
  assert (*cyper_ptr != NULL);

  err = gcry_cipher_setkey (*cyper_ptr,
      password_hash, sizeof (password_hash));
  if (err != 0)
  {
    ERROR ("network plugin: gcry_cipher_setkey returned: %s",
        gcry_strerror (err));
    gcry_cipher_close (*cyper_ptr);
    *cyper_ptr = NULL;
    return (NULL);
  }

  err = gcry_cipher_setiv (*cyper_ptr, iv, iv_size);
  if (err != 0)
  {
    ERROR ("network plugin: gcry_cipher_setkey returned: %s",
        gcry_strerror (err));
    gcry_cipher_close (*cyper_ptr);
    *cyper_ptr = NULL;
    return (NULL);
  }

  return (*cyper_ptr);
} /* }}} int network_get_aes256_cypher */